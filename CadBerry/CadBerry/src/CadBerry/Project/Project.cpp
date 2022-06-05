#include <cdbpch.h>
#include "Project.h"
#include "CadBerry/Log.h"

#include "CadBerry/Utils/SaveFunctions.h"


namespace CDB
{
	void Project::WriteToFile()
	{
		std::ofstream OutputFile;
		OutputFile.open(this->PathToBerryFile, std::ios::out | std::ios::binary);

		int IntVal = BerryProjectVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write berry project version

		IntVal = CDB_MajorVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write CadBerry project version

		IntVal = CDB_MinorVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write CadBerry project minor version

		SaveString(this->Name, OutputFile);

		SaveString(this->PreBuildDir, OutputFile);


		//Precompilation stuff
		OutputFile.write((char*)&this->PrecompileFiles, sizeof(bool));
		OutputFile.write((char*)&this->PrecompilationInterval, sizeof(float));


		//Entry point
		OutputFile.write((char*)&this->MaintainEntryPoint, sizeof(bool));
		
		//Save the target organism
		SaveString(this->TargetOrganism, OutputFile);

		//Save the entry points
		SaveStringVector(EntrySequences, OutputFile);

		//Save the path to the entry point
		SaveString(this->PathToEntryPoint, OutputFile);

		OutputFile.close();
	}

	Project* Project::ReadFromFile(std::string path)
	{
		std::ifstream InputFile;
		InputFile.open(path, std::ifstream::in | std::ifstream::binary);

		int BerryVersion;
		InputFile.read((char*)&BerryVersion, sizeof(int));

		//skip CadBerry info
		{
			int uselessdata;
			InputFile.read((char*)&uselessdata, sizeof(int));
			InputFile.read((char*)&uselessdata, sizeof(int));
		}

		Project* OutputProject = nullptr;
		switch (BerryVersion)
		{
		case 0:
			OutputProject = ReadProject::ReadVersion0(InputFile, path);
			break;
		case 1:
			OutputProject = ReadProject::ReadVersion1(InputFile, path);
			break;
		case 2:
			OutputProject = ReadProject::ReadVersion2(InputFile, path);
			break;
		default:
			CDB_EditorFatal("CadBerry project file at location \"{0}\" shows an unknown project version ({1}). Most likely this file was corrupted.", path, BerryVersion);
			return nullptr;
		}
		OutputProject->PathToBerryFile = path;
		return OutputProject;
	}

	namespace ReadProject
	{
		Project* ReadVersion0(std::ifstream &InputFile, std::string& path)
		{
			Project* NewProject = new Project();
			NewProject->Path = std::filesystem::path(path).parent_path().string();
			CDB_EditorInfo(NewProject->Path);

			LoadStringFromFile(NewProject->Name, InputFile);

			InputFile.close();
			NewProject->PreBuildDir = "\\.GILCache\\";
			return NewProject;
		}

		Project* ReadVersion1(std::ifstream& InputFile, std::string& path)
		{
			Project* NewProject = new Project();
			NewProject->Path = std::filesystem::path(path).parent_path().string();
			CDB_EditorInfo(NewProject->Path);

			LoadStringFromFile(NewProject->Name, InputFile);

			LoadStringFromFile(NewProject->PreBuildDir, InputFile);

			InputFile.close();
			return NewProject;
		}

		Project* ReadVersion2(std::ifstream& InputFile, std::string& path)
		{
			Project* NewProject = new Project();
			NewProject->EntrySequences.clear();
			NewProject->Path = std::filesystem::path(path).parent_path().string();
			CDB_EditorInfo(NewProject->Path);

			LoadStringFromFile(NewProject->Name, InputFile);

			LoadStringFromFile(NewProject->PreBuildDir, InputFile);

			//Precompilation stuff
			InputFile.read((char*)&NewProject->PrecompileFiles, sizeof(bool));
			InputFile.read((char*)&NewProject->PrecompilationInterval, sizeof(float));


			//Entry point
			InputFile.read((char*)&NewProject->MaintainEntryPoint, sizeof(bool));

			//Load the target organism
			LoadStringFromFile(NewProject->TargetOrganism, InputFile);

			//Load the entry points
			LoadStringVector(NewProject->EntrySequences, InputFile);

			//Load the path to the entry point
			LoadStringFromFile(NewProject->PathToEntryPoint, InputFile);

			InputFile.close();
			return NewProject;
		}
	}
}
