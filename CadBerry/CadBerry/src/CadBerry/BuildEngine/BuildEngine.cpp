#include "cdbpch.h"
#include "BuildEngine.h"
#include "CadBerry/Log.h"

namespace CDB
{
	std::string BuildEngine::CreateEntryPoint(std::string& TargetOrganism, std::vector<std::string>& EntrySequences)
	{
		CDB_EditorError("Current build engine doesn't support application-generated entry points");
		return std::string();
	}
}