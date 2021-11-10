#include <gilpch.h>
#include "SaveFunctions.h"

namespace GIL
{
	const char Termination = NULL;

	void SaveString(std::string& Text, std::ofstream& OutputFile)
	{
		int Len = Text.length();
		OutputFile.write((char*)&Len, sizeof(int));
		OutputFile.write((char*)Text.data(), sizeof(char) * Len);
	}

	void LoadStringFromFile(std::string& Text, std::ifstream& InputFile)
	{
		int Len;
		InputFile.read((char*)&Len, sizeof(int));
		Text.resize(Len);
		InputFile.read(Text.data(), sizeof(char) * Len);
	}
}