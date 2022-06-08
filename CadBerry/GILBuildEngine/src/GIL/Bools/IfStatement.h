#pragma once
#include <gilpch.h>
#include "GIL/Parser/Sequence.h"

namespace GIL
{
	class IfStatement
	{
	public:
		IfStatement() {}
		IfStatement(std::string s) : BoolName(std::move(s)) {}

		std::string BoolName;
		std::vector<Sequence*> InnerSequences;
	};
}