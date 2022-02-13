#include "RNAContext.h"

namespace RNAContext
{
	std::string dataPath;
	datatable* SharedDataTable = nullptr;

	void InitRNAContext(std::string DataPath)
	{
		dataPath = DataPath;
		if (SharedDataTable == nullptr)
		{
			SharedDataTable = new datatable();
			//data->opendat(directory, nominal_alphabetName.c_str(), false, skipThermoTables)==0
			SharedDataTable->opendat(dataPath.c_str(), "rna");
		}
	}

	void Cleanup()
	{
		delete SharedDataTable;
	}

	datatable* GetDatatable()
	{
		if (SharedDataTable == nullptr)
		{
			SharedDataTable = new datatable();
			//data->opendat(directory, nominal_alphabetName.c_str(), false, skipThermoTables)==0
			SharedDataTable->opendat(dataPath.c_str(), "rna");
		}
		return SharedDataTable;
	}


	const char* GetDataPath()
	{
		return dataPath.c_str();
	}
}