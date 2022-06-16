#pragma once
#include <gilpch.h>
#include "GIL/Parser/Sequence.h"


#define ExposeModuleToGIL(Module) extern "C"\
{\
	__declspec(dllexport) GIL::GILModule* __stdcall GetModule ()\
	{\
		return new Module;\
	}\
}\

namespace GIL
{
	class GILModule
	{
	public:
		virtual Sequence* GetOperation(std::string OpName) = 0;
		virtual Sequence* GetSequence(std::string SeqName) = 0;
	};
}