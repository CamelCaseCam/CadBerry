#include <cdbpch.h>
#include "SaveFunctions.h"

namespace CDB
{
	const char Termination = NULL;

	void SaveString(std::string& Text, std::ofstream& OutputFile)    //Writes length and chars to file
	{
		int TLen = Text.length();
		OutputFile.write((char*)&TLen, sizeof(int));
		OutputFile.write((char*)Text.data(), sizeof(char) * TLen);
	}

	void LoadStringFromFile(std::string& Text, std::ifstream& InputFile)    //Reads length chars from file
	{
		int TLen = 0;
		InputFile.read((char*)&TLen, sizeof(int));
		Text.resize(TLen);
		InputFile.read(Text.data(), sizeof(char) * TLen);
	}

	void SaveStringVector(std::vector<std::string>& Vec, std::ofstream& OutputFile)
	{
		int TLen = Vec.size();
		OutputFile.write((char*)&TLen, sizeof(int));
		for (std::string& s : Vec)
			SaveString(s, OutputFile);
	}

	void LoadStringVector(std::vector<std::string>& Vec, std::ifstream& InputFile)
	{
		int TLen = 0;
		InputFile.read((char*)&TLen, sizeof(int));
		Vec.reserve(TLen);
		for (int i = 0; i < TLen; ++i)
		{
			std::string tmpstr;
			LoadStringFromFile(tmpstr, InputFile);
			Vec.push_back(std::move(tmpstr));
		}
	}
}