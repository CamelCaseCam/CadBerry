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
		if (CDB::Application::Get().m_BuildEngine == nullptr)
		{
			CDB_EditorError("No build engine loaded");
			return;
		}

		if (OutputTypes.size() == 0)
		{
			std::vector<std::string>* OT = CDB::Application::Get().m_BuildEngine->GetOutputTypes();
			if (OT == nullptr)
			{
				CDB_EditorError("Failed to load build targets from build engine");
				return;
			}
			CDB_EditorInfo((void*)OT);

			OutputTypes.reserve(OT->size());
			std::transform(OT->begin(), OT->end(), std::back_inserter(OutputTypes), convert);
			if (OutputTypes.size() == 0)
			{
				CDB_EditorFatal("The current build engine must provide at least one output type");
			}
		}

		ImGui::ListBox("Project output type", &CurrentOutputType, OutputTypes.data(), OutputTypes.size());
		if (!Application::Get().OpenProject->MaintainEntryPoint)
		{
			ImGui::InputText("Project entry point", &EntryPoint);
		}

		ImGui::InputText("Project output directory", &OutputDirectory);

		if (ImGui::Button("Build Project!"))
		{
			std::string OT = OutputTypes[CurrentOutputType];
			if (Application::Get().OpenProject->MaintainEntryPoint)
			{
				//Generate entry point code
				std::string EntryCode = CDB::Application::Get().m_BuildEngine->CreateEntryPoint(Application::Get().OpenProject->TargetOrganism,
					Application::Get().OpenProject->EntrySequences);

				//Open the file
				std::ofstream OutputFile(CDB::Application::Get().OpenProject->Path + "\\�CadBerryAutogeneratedEntryPoint@" + 
					CDB::Application::Get().OpenProject->Name + "@" + CDB_VersionString + ".gil");
				OutputFile << EntryCode;
				OutputFile.close();

				//Compile the project (entry point)
				std::string Name = "�CadBerryAutogeneratedEntryPoint@" + CDB::Application::Get().OpenProject->Name + "@" + CDB_VersionString;
				CDB::Application::Get().m_BuildEngine->Build(CDB::Application::Get().OpenProject->Path, Name,
					CDB::Application::Get().OpenProject->PreBuildDir, OutputDirectory, OT);
			} else
			{
				CDB::Application::Get().m_BuildEngine->Build(CDB::Application::Get().OpenProject->Path, EntryPoint,
					CDB::Application::Get().OpenProject->PreBuildDir, OutputDirectory, OT);
			}
		}
	}
}