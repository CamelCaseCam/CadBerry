#pragma once
#include <gilpch.h>
#include "GIL/Parser/Region.h"
#include "GIL/Lexer/Token.h"
#include "GIL/Parser/Project.h"

#include "Core.h"

namespace GIL
{
	namespace Compiler
	{
		GILAPI std::pair<std::vector<GIL::Parser::Region>, std::string> Compile(GIL::Parser::Project* Proj,
			std::vector<GIL::Lexer::Token*>* Tokens = nullptr);
	}
}