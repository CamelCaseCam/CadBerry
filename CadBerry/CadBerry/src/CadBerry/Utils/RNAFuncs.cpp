#include <cdbpch.h>
#include "RNAFuncs.h"

#include "RNA.h"

namespace CDB
{
	float GetFreeEnergy(std::string& Code)
	{
		RNA Strand(Code.c_str());
		Strand.FoldSingleStrand(20.0f, 20, 5, "", 30, true);
		return Strand.CalculateFreeEnergy();
	}
}