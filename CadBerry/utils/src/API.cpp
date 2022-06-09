#include <utilspch.h>
#include "GIL.h"

#include "Operations/PrintWarning.h"

#include "CadBerry/Application.h"

std::string DataPath;

namespace utils
{
	class utilsModule : public GIL::GILModule
	{
	public:
		virtual GIL::Sequence* GetOperation(std::string name) override
		{
			if (name == "PrintWarning")
			{
				return Operations::PrintWarning::GetPtr();
			}
			else
			{
				CDB_BuildError("utils module does not contain operation {0}", name);
			}
		}

		virtual GIL::Sequence* GetSequence(std::string name) override
		{
			CDB_BuildError("utils module does not contain sequence {0}", name);
			return nullptr;
		}
	};
}

//Expose the module to GIL
extern "C"
{
	__declspec(dllexport) GIL::GILModule* __stdcall GetModule()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\GILModules\\";
		return new utils::utilsModule();
	}
}