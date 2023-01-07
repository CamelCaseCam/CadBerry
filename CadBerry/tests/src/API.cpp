#include <utilspch.h>
#include "GIL.h"

#include "NumAvailable.h"

#include "CadBerry/Application.h"

std::string DataPath;

class testModule : public GIL::GILModule
{
public:
	virtual GIL::Sequence* GetSequence(std::string name) override
	{
		if (name == "NumAvailable")
		{
			return new NumAvailable();
		}
		return nullptr;
	}
	virtual GIL::Sequence* GetOperation(std::string name) override { return nullptr; }
};

//Expose the module to GIL
extern "C"
{
	__declspec(dllexport) GIL::GILModule* __stdcall GetModule()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\GILModules\\";
		return new testModule();
	}
}