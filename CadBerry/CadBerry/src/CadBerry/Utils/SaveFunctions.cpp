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
}