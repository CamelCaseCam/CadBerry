#pragma once
#include "CadBerry/Core.h"
#include <cdbpch.h>

namespace CDB
{
	CDBAPI void InputStringVector(const std::string& name, std::vector<std::string>& Vec, const char* ElementName = "element", const char* StringName = "Text");
}