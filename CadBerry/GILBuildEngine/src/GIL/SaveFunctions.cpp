#include <gilpch.h>
#include "SaveFunctions.h"

namespace GIL
{
	const char Termination = NULL;

	void SaveString(std::string& Text, std::ofstream& OutputFile)    //Writes length and chars to file
	{
		int Len = Text.length();
		OutputFile.write((char*)&Len, sizeof(int));
		OutputFile.write((char*)Text.data(), sizeof(char) * Len);
	}

	void LoadStringFromFile(std::string& Text, std::ifstream& InputFile)    //Reads length chars from file
	{
		int Len;
		InputFile.read((char*)&Len, sizeof(int));
		Text.resize(Len);
		InputFile.read(Text.data(), sizeof(char) * Len);
	}
}