#include "GIL.h"
#include "TRAPzyme.h"

#include <unordered_map>

namespace sensing
{
	enum class OpName
	{
		SetTRAPzymeTarget,
		GenTZFromAttenuator,
		CreateAttenuator,
	};
	const std::unordered_map<std::string, OpName> Str2OpName = {
		{"SetTRAPzymeTarget", OpName::SetTRAPzymeTarget},
		{"GenTZFromAttenuator", OpName::GenTZFromAttenuator},
		{"CreateAttenuator", OpName::CreateAttenuator},
	};
	class sensingModule : public GIL::GILModule
	{
	public:
		virtual GIL::Operation* GetOperation(std::string name) override
		{
			switch (Str2OpName.at(name))
			{
			case OpName::SetTRAPzymeTarget:
				return SetTRAPzymeTarget::GetPtr();
			case OpName::GenTZFromAttenuator:
				return GenTZFromAttenuator::GetPtr();
			case OpName::CreateAttenuator:
				return CreateAttenuator::GetPtr();
			}
		}

		virtual GIL::Sequence* GetSequence(std::string name) override
		{
			CDB_BuildError("sensing module does not contain sequence {0}", name);
			return nullptr;
		}
	};
}

extern "C"
{
	__declspec(dllexport) GIL::GILModule* __stdcall GetModule()
	{
		return new sensing::sensingModule();
	}
}