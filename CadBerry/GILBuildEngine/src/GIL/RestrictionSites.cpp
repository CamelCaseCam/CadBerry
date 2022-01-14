#include <gilpch.h>
#include "RestrictionSites.h"

namespace GIL
{
	const std::unordered_map<std::string, RestrictionSite> Str2RestrictionSite = {
		{"EcoRI",   RestrictionSite::EcoRI,	  },
		{"EcoRII",  RestrictionSite::EcoRII,   },
		{"BamHI",   RestrictionSite::BamHI,	  },
		{"HindIII", RestrictionSite::HindIII,  },
		{"TaqI",    RestrictionSite::TaqI,	  },
		{"NotI",    RestrictionSite::NotI,	  },
		{"HinFI",   RestrictionSite::HinFI,	  },
		{"Sau3AI",  RestrictionSite::Sau3AI,   },
		{"PvuII",   RestrictionSite::PvuII,	  },
		{"SmaI",    RestrictionSite::SmaI,	  },
		{"HaeIII",  RestrictionSite::HaeIII,   },
		{"HgaI",    RestrictionSite::HgaI,	  },
		{"AluI",    RestrictionSite::AluI,	  },
		{"EcoRV",   RestrictionSite::EcoRV,	  },
		{"EcoP15I", RestrictionSite::EcoP15I,  },
		{"KpnI",    RestrictionSite::KpnI,	  },
		{"PstI",    RestrictionSite::PstI,	  },
		{"SacI",    RestrictionSite::SacI,	  },
		{"SaII",    RestrictionSite::SaII,	  },
		{"ScaI",    RestrictionSite::ScaI,	  },
		{"SpeI",    RestrictionSite::SpeI,	  },
		{"SphI",    RestrictionSite::SphI,	  },
		{"StuI",    RestrictionSite::StuI,	  },
		{"XbaI",    RestrictionSite::XbaI,	  },
	};
}