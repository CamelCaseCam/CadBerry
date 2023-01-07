#pragma once

/*
Data structure representing a bool in a GIL program. While we allow local bools, all bools are really global, so at some point later in 
compilation we'll have to convert the local name "NAME" to the global name "LOCATION@NAME@SEQUENCE@RUN"
*/

#include "GIL/Parser/AST_Node.h"

namespace GIL
{
	namespace Parser
	{
		class BoolImplementation;
		
		struct GILBool
		{
			GILBool(std::string Name) : Name(Name) {}
			std::string Name;
			
			//Implementation is set by the compiler
			BoolImplementation* impl = nullptr;
		};
	}
}