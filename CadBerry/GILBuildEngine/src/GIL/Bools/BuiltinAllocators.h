#pragma once
#include "BoolAllocator.h"

namespace GIL
{
	namespace Compiler
	{
		class NaiveAllocator : public BoolAllocator
		{
		public:
			virtual void AllocateBools(Parser::Project* Proj) override;
		};
	}
}