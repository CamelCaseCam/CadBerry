#pragma once
#include <gilpch.h>

namespace GIL
{
	enum class RestrictionSite
	{
		EcoRI,
		EcoRII,
		BamHI,
		HindIII,
		TaqI,
		NotI,
		HinFI,
		Sau3AI,
		PvuII,
		SmaI,
		HaeIII,
		HgaI,
		AluI,
		EcoRV,
		EcoP15I,
		KpnI,
		PstI,
		SacI,
		SaII,
		ScaI,
		SpeI,
		SphI,
		StuI,
		XbaI,
	};
	extern const std::unordered_map<std::string, RestrictionSite> Str2RestrictionSite;
}