#include "CadBerry.h"

#include "src/Viewport.h"


class __declspec(dllexport) DeepMDModule : public CDB::Module
{
public:
	DeepMDModule() : CDB::Module("DeepMD module") {}
	virtual ~DeepMDModule() override {}
	
	UseImGui
	
	std::string viewports[1] = { "DeepMD" };
	
	virtual std::string* GetViewportNames() override
	{
		NumViewports = 1;
		return viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string name) override
	{
		if (name == "DeepMD")
		{
			return new dmd::DeepMDViewport();
		}
		CDB_EditorError("Viewport {0} does not exist", name);
		return nullptr;
	}
};

//Expose the module to CadBerry
extern "C"
{
	__declspec(dllexport) CDB::Module* __stdcall GetModule()
	{
		return new DeepMDModule();
	}
}