#include <cdbpch.h>
#include "Project.h"
#include "CadBerry/Log.h"


namespace CDB
{
	void Project::WriteToFile()
	{
		std::ofstream OutputFile;
		OutputFile.open(this->Path + "/" + this->Name + ".berry", std::ios::out | std::ios::binary);

		int IntVal = BerryProjectVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write berry project version

		IntVal = CDB_MajorVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write CadBerry project version

		IntVal = CDB_MinorVersion;
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write CadBerry project minor version

		IntVal = this->Name.length();
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write the length of name to the file

		OutputFile.write(this->Name.data(), sizeof(char) * IntVal);    //Write the string to file

		IntVal = this->PreBuildDir.length();
		OutputFile.write((char*)&IntVal, sizeof(int));    //Write the length of Prebuild path to the file

		OutputFile.write(this->PreBuildDir.data(), sizeof(char) * IntVal);    //Write the string to file

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

		switch (BerryVersion)
		{
		case 0:
			return ReadProject::ReadVersion0(InputFile, path);
		case 1:
			return ReadProject::ReadVersion1(InputFile, path);
		default:
			CDB_EditorFatal("CadBerry project file at location \"{0}\" shows an unknown project version ({1}). Most likely this file was corrupted.", path, BerryVersion);
			return nullptr;
		}
	}

	namespace ReadProject
	{
		Project* ReadVersion0(std::ifstream &InputFile, std::string& path)
		{
			Project* NewProject = new Project();
			NewProject->Path = std::filesystem::path(path).parent_path().string();
			CDB_EditorInfo(NewProject->Path);

			int NumChars;
			InputFile.read((char*)&NumChars, sizeof(int));
			NewProject->Name.reserve(NumChars);
			CDB_EditorInfo(NumChars);
			
			InputFile.read((char*)NewProject->Name.data(), sizeof(char) * NumChars);

			InputFile.close();
			NewProject->PreBuildDir = "\\.GILCache\\";
			return NewProject;
		}

		Project* ReadVersion1(std::ifstream& InputFile, std::string& path)
		{
			Project* NewProject = new Project();
			NewProject->Path = std::filesystem::path(path).parent_path().string();
			CDB_EditorInfo(NewProject->Path);

			int NumChars;
			InputFile.read((char*)&NumChars, sizeof(int));
			NewProject->Name.reserve(NumChars);
			CDB_EditorInfo(NumChars);

			InputFile.read(NewProject->Name.data(), sizeof(char) * NumChars);

			NumChars = 0;
			InputFile.read((char*)&NumChars, sizeof(int));
			NewProject->PreBuildDir.reserve(NumChars);

			InputFile.read(NewProject->PreBuildDir.data(), sizeof(char) * NumChars);

			InputFile.close();
			return NewProject;
		}
	}
}
