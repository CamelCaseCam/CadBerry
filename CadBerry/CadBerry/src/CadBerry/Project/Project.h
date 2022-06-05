#pragma once

#include <string>
#include "CadBerry/Core.h"

//.berry defines
#define BerryProjectVersion 2

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

		std::string PathToBerryFile;    //Allows you to change the name but keep the same file

		bool PrecompileFiles = true;
		float PrecompilationInterval = 30.0f;

		bool MaintainEntryPoint = true;

		//If entry point
		std::string TargetOrganism = "";
		std::vector<std::string> EntrySequences = { "main" };
		
		//If no entry point
		std::string PathToEntryPoint = "";


		Project() {}

		Project(std::string Name, std::string Path)
		{
			this->Name = Name;
			this->Path = Path;
			this->PathToBerryFile = Path + "/" + Name + ".berry";
		}

		~Project() {}

		void WriteToFile();
		static Project* ReadFromFile(std::string path);
	};

	namespace ReadProject
	{
		Project* ReadVersion0(std::ifstream &InputFile, std::string& path);
		Project* ReadVersion1(std::ifstream& InputFile, std::string& path);
		Project* ReadVersion2(std::ifstream& InputFile, std::string& path);
	}
}