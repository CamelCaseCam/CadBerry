#include <cdbpch.h>
#include "Update.h"
#include "LoadPackages.h"
#include "CadBerry/Application.h"
#include "CadBerry/ExecuteCommand.h"

#pragma warning(push, 0)
#include "imgui.h"
#pragma warning(pop)

#define YAML_CPP_STATIC_DEFINE

#include "yaml-cpp/yaml.h"

#include <sys/stat.h>
#include <stdlib.h>

namespace CDB
{
	void ShowPackageManager()
	{
		if (ImGui::CollapsingHeader("Out of date"))
		{
			for (Package& pkg : Packages)
			{
				if (pkg.changed)
				{
					ImGui::Text(("Name: " + pkg.Name).c_str());
					ImGui::Text(("Last updated: " + pkg.Downloaded).c_str());
					if (ImGui::Button("Update"))
					{
						//Update the package download time
						{
							YAML::Node Package = YAML::LoadFile(pkg.PathToPackageDefinition);
							Package["Downloaded"] = pkg.NewUpdateTime;
							YAML::Emitter Out;
							Out << Package;
							std::ofstream OutFile(pkg.PathToPackageDefinition);
							OutFile << Out.c_str();
						}

						std::string tmp1 = "\"https://github.com/" + pkg.Owner + "/" + pkg.Repo + "/raw/" + pkg.Branch + "/" + pkg.Path 
							+ "\" \"" + Application::Get().PathToEXE + "/\" " + std::to_string(pkg.Files.size()) + " ";
						for (std::string& file : pkg.Files)
						{
							tmp1 += "\"" + file + "\" ";
						}
						CDB_BuildInfo(tmp1);

						std::string tmp3 = Application::Get().PathToEXE + "/CadBerry_updater";
						ExecCommand(tmp3, tmp1);
						exit(0);
					}
				}
			}
		}

		if (ImGui::CollapsingHeader("Up to date"))
		{
			for (Package& pkg : Packages)
			{
				if (!pkg.changed)
				{
					ImGui::Text(("Name: " + pkg.Name).c_str());
					ImGui::Text(("Last updated: " + pkg.Downloaded).c_str());
				}
			}
		}
	}
}