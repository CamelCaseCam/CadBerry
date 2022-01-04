#pragma once
#include <gilpch.h>

#include "Core.h"

namespace GIL
{
	namespace utils
	{
		GILAPI std::pair<std::string, int> Find(std::string& Code, std::string pattern, int start = 0);
		GILAPI std::string GetComplement(std::string& Code);
		GILAPI std::string GetReverseComplement(std::string& Code);
		GILAPI float GetGCRAtio(const std::string& Code);
		GILAPI float GetAGRatio(const std::string& Code);
		
		GILAPI std::string GenWithPattern(std::string& pattern);
	}
}