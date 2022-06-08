#pragma once
#include <gilpch.h>
#include "Token.h"

#include "Core.h"

namespace GIL
{
	namespace Lexer
	{
		GILAPI std::vector<Token*>* Tokenize(std::string& Text);
	}
}