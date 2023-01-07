#pragma once

#include "GIL/Parser/Project.h"
#include "BoolAllocator.h"
#include <gilpch.h>

namespace GIL
{
	namespace Compiler
	{
		extern std::unordered_map<std::string, BoolAllocator*> boolAllocators;
		extern BoolAllocator* CurrentAllocator;
		void GenerateBoolGraphs(Parser::Project* Proj);
	}
}