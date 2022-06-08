#include <cdbpch.h>
#include "CadBerry/Log.h"
#include "LoadPackages.h"
#include "CadBerry/Application.h"

#define YAML_CPP_STATIC_DEFINE

#include "yaml-cpp/yaml.h"
#include <cpr/cpr.h>

#define IsDate(str) str[i + 1] == 'd' && str[i + 2] == 'a' && str[i + 3] == 't' && str[i + 4] == 'e' && str[i + 5] == '"' && str[i + 6] == ':'

namespace CDB
{
	std::vector<Package> Packages;

	void GetPackages(std::string& DataPath)
	{
		Packages.clear();
		std::vector<std::filesystem::path> PackagePaths;
		for (auto const& dir_entry : std::filesystem::directory_iterator{ DataPath })    //Load packages
		{
			if (dir_entry.path().extension() == ".cdbpkg")
			{
				PackagePaths.push_back(dir_entry.path());
			}
		}

		for (std::filesystem::path& path : PackagePaths)    //Load YAML and check if it's up to date
		{
			YAML::Node package = YAML::LoadFile(path.string());

			YAML::Node sourceinfo = package["Source"];
			
			//Github API call, url format: https://api.github.com/repos/[owner]/[repo]/commits/[branch]?page=1&per_page=1
			std::string SourceURL = "https://api.github.com/repos/" + sourceinfo["Owner"].as<std::string>() + "/" + 
				sourceinfo["Repo"].as<std::string>() + "/commits/" + sourceinfo["Branch"].as<std::string>() + "?page=1&per_page=1";

			cpr::Response r = cpr::Get(cpr::Url{ SourceURL }, cpr::Timeout{ 10000 });
			if (r.status_code != 200)
				continue;

			Packages.push_back({ package["Name"].as<std::string>(), package["Version"].as<std::string>(),
				package["Downloaded"].as<std::string>(), sourceinfo["Owner"].as<std::string>(), sourceinfo["Repo"].as<std::string>(),
				sourceinfo["Branch"].as<std::string>(), sourceinfo["Path"].as<std::string>(), package["Files"].as<std::vector<std::string>>() });
			
			std::string Date;
			Date.reserve(20);    //Date string will always be 20 chars long
			//Find the date
			for (int i = 0; i < r.text.size() - 8; ++i)    //8 chars from the start of the date key to the value
			{
				if (r.text[i] == '"')
				{
					if (IsDate(r.text))
					{
						i += 9;
						for (i; i < r.text.size() && r.text[i] != '"'; ++i)
						{
							Date += r.text[i];
						}
						break;
					}
				}
			}
			CDB_EditorInfo("Module last updated at time {0}", Date);
			Package& pkg = Packages[Packages.size() - 1];
			pkg.changed = pkg.Downloaded != Date;
			if (pkg.changed)
			{
				Application::Get().ShowPackages = true;
				pkg.NewUpdateTime = Date;
				pkg.PathToPackageDefinition = path.string();
			}
		}
	}
}