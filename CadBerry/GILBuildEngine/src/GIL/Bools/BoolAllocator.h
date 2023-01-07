#pragma once
#include "GIL/Parser/Project.h"


namespace GIL
{
	namespace Compiler
	{
		class BoolAllocator
		{
		public:
			BoolAllocator() {}
			virtual ~BoolAllocator() {}

			virtual void AllocateBools(Parser::Project* Proj) = 0;
		};
	}
}
