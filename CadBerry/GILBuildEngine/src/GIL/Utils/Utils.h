#pragma once
#include <gilpch.h>

#include "Core.h"
#include "GIL/RestrictionSites.h"

namespace GIL
{
	namespace utils
	{
		GILAPI std::pair<std::string, int> Find(std::string& Code, std::string pattern, int start = 0);
		GILAPI std::string GetComplement(const std::string& Code);
		GILAPI std::string GetReverseComplement(const std::string& Code);
		GILAPI float GetGCRAtio(const std::string& Code);
		GILAPI float GetAGRatio(const std::string& Code);
		
		GILAPI std::string GenWithPattern(std::string& pattern);
		GILAPI std::string GenRandomBases(int length);

		GILAPI bool FindNearby(std::string& Code1, std::string& Code2, std::string& Pattern);

		GILAPI std::string GetRestrictionSite(RestrictionSite rs);
		GILAPI std::string GetRestrictionSite(std::string& rs);

		GILAPI std::pair<std::string, std::string> FindSequenceInIntermediates(std::string& SequenceName);
	}
}