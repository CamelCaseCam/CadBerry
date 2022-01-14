#pragma once

#include <string>
#include "CadBerry/Core.h"

//.berry defines
#define BerryProjectVersion 1

namespace CDB
{
	struct ProjInfo
	{
		std::string Name;
		std::string Path;
	};

	class CDBAPI Project
	{
	public:
		std::string Name;
		std::string Path;
		std::string PreBuildDir = "\\.GILCache\\";

		Project() {}

		Project(std::string Name, std::string Path)
		{
			this->Name = Name;
			this->Path = Path;
		}

		~Project() {}

		void WriteToFile();
		static Project* ReadFromFile(std::string path);
	};

	namespace ReadProject
	{
		Project* ReadVersion0(std::ifstream &InputFile, std::string& path);
		Project* ReadVersion1(std::ifstream& InputFile, std::string& path);
	}
}