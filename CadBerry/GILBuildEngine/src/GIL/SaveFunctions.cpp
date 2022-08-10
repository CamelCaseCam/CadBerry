#include <gilpch.h>
#include "SaveFunctions.h"

namespace GIL
{
	const char Termination = NULL;

	void SaveString(const std::string& Text, std::ofstream& OutputFile)    //Writes length and chars to file
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
	void SaveStringVector(const std::vector<std::string>& Vec, std::ofstream& OutputFile)
	{
		//Save the number of elements in the vector
		size_t NumElements = Vec.size();
		OutputFile.write((char*)&NumElements, sizeof(size_t));
		//Save each element in the vector
		for (const std::string& s : Vec)
		{
			SaveString(s, OutputFile);
		}
	}
	
	void LoadStringVectorFromFile(std::vector<std::string>& Vec, std::ifstream& InputFile)
	{
		size_t NumElements = 0;
		InputFile.read((char*)&NumElements, sizeof(size_t));
		Vec.reserve(NumElements);
		std::string buff;
		for (int i = 0; i < NumElements; i++)
		{
			LoadStringFromFile(buff, InputFile);
			Vec.push_back(buff);
		}
	}

	
	void SaveSize(size_t Size, std::ofstream& OutputFile)
	{
		OutputFile.write((char*)&Size, sizeof(size_t));
	}
	
	void LoadSizeFromFile(size_t& Size, std::ifstream& InputFile)
	{
		InputFile.read((char*)&Size, sizeof(size_t));
	}
	size_t LoadSizeFromFile(std::ifstream& InputFile)
	{
		size_t tmp = 0;
		InputFile.read((char*)&tmp, sizeof(size_t));
		return tmp;
	}
}