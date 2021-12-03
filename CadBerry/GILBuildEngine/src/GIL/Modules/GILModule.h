#pragma once
#include <gilpch.h>
#include "GIL/Parser/Operation.h"
#include "GIL/Parser/Sequence.h"

namespace GIL
{
	class GILModule
	{
	public:
		virtual Operation* GetOperation(std::string OpName) = 0;
		virtual Sequence* GetSequence(std::string SeqName) = 0;
	};
}