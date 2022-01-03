#pragma once
#include <gilpch.h>
#include "Core.h"

namespace GIL
{
	GILAPI void SaveString(std::string& Text, std::ofstream& OutputFile);
	GILAPI void LoadStringFromFile(std::string& Text, std::ifstream& InputFile);
}