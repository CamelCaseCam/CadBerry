#pragma once
#include <gilpch.h>

namespace GIL
{
	void SaveString(std::string& Text, std::ofstream& OutputFile);
	void LoadStringFromFile(std::string& Text, std::ifstream& InputFile);
}