#pragma once
#include <gilpch.h>
#include "Core.h"

namespace GIL
{
	GILAPI void SaveString(const std::string& Text, std::ofstream& OutputFile);
	GILAPI void LoadStringFromFile(std::string& Text, std::ifstream& InputFile);

	GILAPI void SaveStringVector(const std::vector<std::string>& Vec, std::ofstream& OutputFile);
	GILAPI void LoadStringVectorFromFile(std::vector<std::string>& Vec, std::ifstream& InputFile);

	GILAPI void SaveSize(size_t Size, std::ofstream& OutputFile);	
	GILAPI void LoadSizeFromFile(size_t& Size, std::ifstream& InputFile);
	GILAPI size_t LoadSizeFromFile(std::ifstream& InputFile);
}