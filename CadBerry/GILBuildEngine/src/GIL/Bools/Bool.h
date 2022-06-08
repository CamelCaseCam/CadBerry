#pragma once
#include "GIL/Parser/Operation.h"
#include "GIL/Parser/Sequence.h"

namespace GIL
{
	class Bool;    //Forward declaration

	class Bool
	{
	public:
		virtual Operation* Get() = 0;
		virtual Sequence* Condition() = 0;

		//Output is a pointer reference so that you can change it from within the function if the function modifies its own state
		virtual std::string& AND(Bool* Other, Bool*& Output) = 0;
		virtual std::string& OR(Bool* Other, Bool*& Output) = 0;
		virtual std::string& NOT(Bool*& Output) = 0;
		virtual std::string& NAND(Bool* Other, Bool*& Output) = 0;
		virtual std::string& NOR(Bool* Other, Bool*& Output) = 0;
	};
}