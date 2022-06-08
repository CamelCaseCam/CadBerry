#pragma once
#include <cdbpch.h>
#include "Package.h"

namespace CDB
{
	extern std::vector<Package> Packages;
	void GetPackages(std::string& DataPath);
}