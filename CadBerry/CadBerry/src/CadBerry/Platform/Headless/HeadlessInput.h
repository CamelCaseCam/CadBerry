#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

namespace CDB
{
	extern CDBAPI std::vector<std::string> Inputs;
	extern CDBAPI size_t CurrentInputInVector;
	std::string GetInput();
}