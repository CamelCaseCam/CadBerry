#include <cdbpch.h>
#include "CadBerry/Application.h"
#include "CadBerry/Log.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace CDB
{
	std::string EntryPoint;
	std::string OutputDirectory;

	int CurrentOutputType;
	std::vector<const char*> OutputTypes;

	const char* convert(const std::string& s)
	{
		return s.c_str();
	}
	
	void BuildDialog()
	{
		if (OutputTypes.size() == 0)
		{
			std::vector<std::string>* OT = CDB::Application::Get().m_BuildEngine->GetOutputTypes();
			OutputTypes.reserve(OT->size());
			std::transform(OT->begin(), OT->end(), std::back_inserter(OutputTypes), convert);
			if (OutputTypes.size() == 0)
			{
				CDB_EditorFatal("The current build engine must provide at least one output type");
			}
		}

		ImGui::ListBox("Project output type", &CurrentOutputType, OutputTypes.data(), OutputTypes.size());
		ImGui::InputText("Project entry point", &EntryPoint);
		ImGui::InputText("Project output directory", &OutputDirectory);

		if (ImGui::Button("Build Project!"))
		{
			std::string OT = OutputTypes[CurrentOutputType];
			CDB::Application::Get().m_BuildEngine->Build(CDB::Application::Get().OpenProject->Path, EntryPoint,
				CDB::Application::Get().OpenProject->PreBuildDir, OutputDirectory, OT);
		}
	}
}