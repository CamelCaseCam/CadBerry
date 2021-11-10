#pragma once
#include "Compiler.h"
#include "GIL/Parser/Project.h"

namespace GIL
{
	namespace Compiler
	{
		namespace GBSequence
		{
			std::string WriteToString(std::pair<std::vector<GIL::Parser::Region>, std::string> Output, 
				std::string& FileName, GIL::Parser::Project* Proj);
		}
	}
}