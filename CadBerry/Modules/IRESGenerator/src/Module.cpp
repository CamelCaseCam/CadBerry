#include "CadBerry.h"

#define IRES_DLL
#include "GenerateIRES.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ImPlot.h"

#include <time.h>

CDB::Application* CDBApp;


class __declspec(dllexport) IRESDesigner : public CDB::Viewport
{
public:
	IRESDesigner() : CDB::Viewport("IRES Designer") {};
	~IRESDesigner() override {};
	
	int GenerationSize = 100;
	int NumGenerations = 100;
	int NewPerGeneration = 3;
	int PreservePerGeneration = 5;
	int AttenuatorSizeRange[2] = { 20, 35 };
	std::string AntiAttenuator;
	std::pair<bool, std::future<void*>> OutputAttenuator;
	bool SubmittedTask = false;
	std::string Output = "No attenuator generated";
	int CurrentGeneration = 0;

#define MaxOverlap 5    //TODO: Make this editable in the future
	virtual void GUIDraw() override
	{
		ImGui::InputInt("Generation size", &GenerationSize);
		ImGui::InputInt("Number of generations", &NumGenerations);
		ImGui::InputInt("Random attenuators per generation", &NewPerGeneration);
		ImGui::InputInt("Number of best attenuators to add to the next generation", &PreservePerGeneration);
		ImGui::InputInt2("Attenuator size range", AttenuatorSizeRange);
		ImGui::InputText("Target sequence", &AntiAttenuator);
		if (ImGui::Button("Generate IRES sensor") && !SubmittedTask)
		{
			IRESDesign::IRESParams* Params = new IRESDesign::IRESParams(GenerationSize, NumGenerations, NewPerGeneration, PreservePerGeneration, AttenuatorSizeRange[0], AttenuatorSizeRange[1], MaxOverlap, &CurrentGeneration, std::move(AntiAttenuator));
			OutputAttenuator = CDB::ThreadPool::Get()->AddStandardTask(IRESDesign::CreateIRESThread, Params);
			SubmittedTask = OutputAttenuator.first;
		}
		if (SubmittedTask)
		{
			if (OutputAttenuator.second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				Output = *(std::string*)OutputAttenuator.second.get();
				SubmittedTask = false;
			}
			else
			{
				ImGui::Text("Running genetic algorithm. Generation %i/%i", CurrentGeneration, NumGenerations);
			}
		}

		ImGui::Text("Algorithm output:");
		ImGui::Text(Output.c_str());

		if (ImGui::Button("Copy output"))
		{
			ImGui::SetClipboardText(Output.c_str());
		}
	}

	virtual void Start() override
	{

	}
	virtual void OnClose() override {}
};

class __declspec(dllexport) IRESModule : public CDB::Module
{
public:
	IRESModule() : CDB::Module("IRES designer module")
	{
		CDBApp = &CDB::Application::Get();
	}
	~IRESModule() override {}

	UseImGui

	std::string Viewports[1] = { "IRES Designer" };
	virtual std::string* GetViewportNames() override
	{
		NumViewports = 1;
		return Viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string viewport) override
	{
		return new IRESDesigner();
	}
};

//Expose the module to CadBerry
extern "C"
{
	__declspec(dllexport) CDB::Module* __stdcall GetModule()
	{
		return new IRESModule();
	}
}