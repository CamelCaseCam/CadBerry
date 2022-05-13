#pragma once
#include <string>
#include "Core.h"
#include "src/rna_library.h"

namespace RNAContext
{
	//Must be called before any RNA functions
	RNAPI void InitRNAContext(std::string DataPath);
	RNAPI void Cleanup();

	RNAPI const char* GetDataPath();

	RNAPI datatable* GetDatatable();
}
