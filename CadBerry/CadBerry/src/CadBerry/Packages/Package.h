#pragma once
#include <cdbpch.h>

namespace CDB
{
	struct Package
	{
		std::string Name;
		std::string Version;
		std::string Downloaded;
		std::string Owner;
		std::string Repo;
		std::string Branch;
		std::string Path;
		bool changed = false;

		std::vector<std::string> Files;

		Package(std::string name, std::string version, std::string downloaded, std::string owner, std::string repo, std::string branch,
			std::string path, std::vector<std::string> files) : Name(name), Version(version), Downloaded(downloaded), Owner(owner),
			Repo(repo), Branch(branch), Path(path), Files(files) {}
	};
}