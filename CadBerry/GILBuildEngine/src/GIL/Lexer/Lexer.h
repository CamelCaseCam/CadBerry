#pragma once
#include <gilpch.h>
#include "Token.h"

namespace GIL
{
	namespace Lexer
	{
		std::vector<Token*>* Tokenize(std::string& Text);
	}
}