#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

namespace CDB
{
	CDBAPI void SaveString(std::string& Text, std::ofstream& OutputFile);
	CDBAPI void LoadStringFromFile(std::string& Text, std::ifstream& InputFile);

	CDBAPI void SaveStringVector(std::vector<std::string>& Vec, std::ofstream& OutputFile);
	CDBAPI void LoadStringVector(std::vector<std::string>& Vec, std::ifstream& InputFile);
}