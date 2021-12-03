#define FMT_HEADER_ONLY
#include "CadBerry.h"
#include "CadBerry/Project/Project.h"

#include "Complement.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <filesystem>
#include <stdlib.h>
#include <sys/stat.h>

CDB::Application* CDBApp;

class __declspec(dllexport) DNAEditor : public CDB::Viewport
{
public:
	DNAEditor() : CDB::Viewport("DNA Editor") {};
	~DNAEditor() override {};

	int DNAOperation = 0;
	const std::vector<const char*> DNAOperations = {
		"Reverse Complement",
		"Complement",
		"Reverse strand"
	};
	std::string DNA;
	std::string Output;

	virtual void Draw() override
	{
		ImGui::ListBox("DNA transformation", &DNAOperation, DNAOperations.data(), DNAOperations.size());
		ImGui::InputText("DNA", &DNA);
		if (ImGui::Button("Transform"))
		{
			switch (DNAOperation)
			{
			case 0:
				Core::GetReverseComplement(DNA, Output);
				break;
			case 1:
				Core::GetComplement(DNA, Output);
				break;
			case 2:
				Core::Reverse(DNA, Output);
				break;
			default:
				break;
			}
		}
		ImGui::Text(Output.c_str());
		if (ImGui::Button("Copy output"))
		{
			ImGui::SetClipboardText(Output.c_str());
		}
	}

	virtual void Start() override {}
	virtual void OnClose() override {}
};

enum class ViewportType
{
	DNAEditor
};

std::unordered_map<std::string, ViewportType> Name2Viewport({
	{ "DNA Editor", ViewportType::DNAEditor }
});

class __declspec(dllexport) CoreModule : public CDB::Module
{
public:
	CoreModule() : CDB::Module("Core CadBerry module") {}
	~CoreModule() override {}

	UseImGui
	
	std::string Viewports[1] = { "DNA Editor" };
	virtual std::string* GetViewportNames() override
	{
		NumViewports = 1;
		return Viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string viewport) override
	{
		switch (Name2Viewport[viewport])
		{
		case ViewportType::DNAEditor:
			return new DNAEditor();
		default:
			return nullptr;
		}
	}
};

//Expose the module to CadBerry
extern "C"
{
	__declspec(dllexport) CDB::Module* __stdcall GetModule()
	{
		return new CoreModule();
	}
}