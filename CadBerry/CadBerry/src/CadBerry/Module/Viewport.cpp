#include <cdbpch.h>
#include "Viewport.h"
#include "CadBerry/Log.h"
#include "CadBerry/Module/Module.h"
#include "CadBerry/Application.h"
#include "CadBerry/BuildEngine/BuildDialog.h"
#include "CadBerry/DNAVis/DisplayDNA.h"
#include "CadBerry/RenderUtils/RichText.h"
#include "CadBerry/Rendering/Utils/Utils.h"
#include "CadBerry/RenderUtils/InputVector.h"
#include "CadBerry/Platform/Headless/HeadlessInput.h"

#include <GLFW/glfw3.h>

#pragma warning(push, 0)
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#pragma warning(pop)

namespace CDB
{
	DNAVisualization Vis;
	void ViewportLayer::OnAttach()
	{
		//Vis = DNAVisualization("AAAAATTTAATATTTATTTATATATATATTTTATATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", { {1, 6, "Test region"}, {9, 15, "Test region 2"}, {16, 25, "Test region 2"} });
	}

	void ViewportLayer::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("CadBerry Editor window", NULL, flags);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				bool NewProject = false;
				bool OpenSettingsWindow = false;
				bool BuildProject = false;
				ImGui::MenuItem("New Project", NULL, &NewProject);
				if (NewProject)
				{
					Application::Get().NewProj = true;
					Application::Get().ShouldExit = true;    //this will make us exit the editor window and open a project creation window
				}
				ImGui::MenuItem("Settings", NULL, &OpenSettingsWindow);

				if (OpenSettingsWindow)
					OpenSettings = true;

				ImGui::MenuItem("Build", NULL, &BuildProject);
				if (BuildProject)
				{
					Application::Get().ShowBuildWindow = true;
				}

				ImGui::EndMenu();
			}
			
			
			if (ImGui::BeginMenu("Windows"))
			{
				for (Module* module : Application::Get().Modules)
				{
					std::string* s = module->GetViewportNames();
					bool CreateWindow = false;
					for (int i = 0; i < module->NumViewports; ++i)
					{
						ImGui::MenuItem(s[i].c_str(), NULL, &CreateWindow);
						if (CreateWindow)
						{
							Viewport* NewViewport = module->CreateViewport(s[i]);
							Application::Get().AddViewport(NewViewport);
							NewViewport->Start();
							CreateWindow = false;
						}
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		if (OpenSettings)
		{
			ImGui::Begin("Settings", &OpenSettings);
			SettingsWindow();
			ImGui::End();
		}

		Viewport* ToBeDeleted = nullptr;
		for (Viewport* l_viewport : OpenViewports)
		{
			l_viewport->Update(io.DeltaTime);
			if (l_viewport->Background) continue;

			ImGui::Begin(l_viewport->Name.c_str(), &l_viewport->IsOpen);
			l_viewport->GUIDraw();
			ImGui::End();
			if (!l_viewport->IsOpen)    //If the user has closed the viewport, mark that viewport to be deleted. 
			{
				ToBeDeleted = l_viewport;    //Since this is called every frame, it's impossible for the user to close more than one viewport. 
				l_viewport->OnClose();
			}
		}

		if (ToBeDeleted != nullptr)
		{
			OpenViewports.erase(std::remove(OpenViewports.begin(), OpenViewports.end(), ToBeDeleted), OpenViewports.end());
			delete ToBeDeleted;
		}
	}

	void ViewportLayer::Draw()
	{
		for (Viewport* viewport : OpenViewports)
		{
			viewport->Draw();
		}
	}

	ViewportLayer::~ViewportLayer()
	{
		for (Viewport* vp : OpenViewports)
		{
			vp->OnClose();
			delete vp;
		}
	}

	//Forward declarations
	void ProjectMenu();
	void WindowMenu();
	void SettingsMenu();
	void BuildMenu();

	extern const char* HelpString;

	void ViewportLayer::HeadlessInput()
	{
		using std::cout, std::cin, std::endl;


		std::string CurrentInput;
		cout << "Enter a command: ";
		CurrentInput = GetInput();

		if (CurrentInput == "project")
			ProjectMenu();
		else if (CurrentInput == "windows")
			WindowMenu();
		else if (CurrentInput == "window list")
		{
			cout << "List of open windows:";
			for (Viewport* viewport : this->OpenViewports)
				cout << "\n" << viewport->Name;
			cout << endl;
		}
		else if (CurrentInput == "help")
			cout << HelpString << endl;
		else if (CurrentInput == "exit")
			CDB::Application::Get().ShouldExit = true;
		else
		{
			Viewport* ToBeDeleted = nullptr;

			//Search the open windows to see if CurrentInput matches
			for (Viewport* viewport : this->OpenViewports)
			{
				if (viewport->Name == CurrentInput)
				{
					viewport->HeadlessInput();
					if (!viewport->IsOpen)
					{
						ToBeDeleted = viewport;
						viewport->OnClose();
					}
					break;
				}
			}

			if (ToBeDeleted != nullptr)
			{
				OpenViewports.erase(std::remove(OpenViewports.begin(), OpenViewports.end(), ToBeDeleted), OpenViewports.end());
				delete ToBeDeleted;
			}
		}
	}

	void ProjectMenu()
	{
		using std::cout, std::cin, std::endl;


		std::string CurrentInput;
		cout << 
R"(----------------------------
   CadBerry Project Menu
----------------------------

Type "new project" to open or create a new project
Type "settings" to edit project settings
Type "build" to build the project
Type "exit" to exit this menu)" << endl;

		CurrentInput = GetInput();
		if (CurrentInput == "new project")
		{
			Application::Get().NewProj = true;
			Application::Get().ShouldExit = true;    //this will make us exit the editor window and open a project creation window
		}
		else if (CurrentInput == "settings")
			SettingsMenu();
		else if (CurrentInput == "build")
			BuildMenu();
		else
			cout << "Exiting CadBerry Project Menu" << endl;
	}

#define IsYes(Input) Input == "y" || Input == "Y"
#define IsNo(Input) Input == "n" || Input == "N"
	void SettingsMenu()
	{
		Project* proj = CDB::Application::Get().OpenProject.raw();
		using std::cout, std::cin, std::endl;

		cout << 
R"(----------------------------
     CadBerry Settings
----------------------------
)" << endl;

		cout << "Change project name? [y/N] ";
		std::string CurrentInput = GetInput();
		if (IsYes(CurrentInput))
		{
			cout << "Enter new project name: ";
			proj->Name = GetInput();
		}

		cout << "Precompile files? [Y/n] ";
		CurrentInput = GetInput();
		proj->PrecompileFiles = !(IsNo(CurrentInput));

		if (proj->PrecompileFiles)
		{
			cout << "\nEnter precompilation interval in seconds: ";
			proj->PrecompilationInterval = std::stof(GetInput());

			cout << "Enter precompilation directory: ";
			proj->PreBuildDir = GetInput();
		}

		cout << "Create entry point? [Y/n] ";
		CurrentInput = GetInput();
		proj->MaintainEntryPoint = !(IsNo(CurrentInput));

		if (proj->MaintainEntryPoint)
		{
			cout << "\nEnter your target organism: ";
			proj->TargetOrganism = GetInput();
			cout << "\nHow many sequences should be compiled? ";
			int NumSeqs = std::stoi(GetInput());

			proj->EntrySequences.resize(0);
			proj->EntrySequences.reserve(NumSeqs);

			for (int i = 0; i < NumSeqs; ++i)
			{
				cout << "Enter the name of sequence " << i + 1 << ": ";
				proj->EntrySequences.push_back(GetInput());
			}
		}
		else
		{
			cout << "Enter the relative path to the project entry point: ";
			proj->PathToEntryPoint = GetInput();
		}
	}

	const char* convert(const std::string& s);

	extern std::vector<const char*> OutputTypes;
	extern std::string Distribution;
	void BuildMenu()
	{
		Project* proj = CDB::Application::Get().OpenProject.raw();
		using std::cout, std::cin, std::endl;

		if (CDB::Application::Get().m_BuildEngine == nullptr)
		{
			CDB_EditorError("No build engine loaded");
			return;
		}

		//OutputTypes caches the build engine's output types, if it's size is 0 we need to load the output types
		if (OutputTypes.size() == 0)
		{
			//Load valid build targets from the build engine
			std::vector<std::string>* OT = CDB::Application::Get().m_BuildEngine->GetOutputTypes();
			if (OT == nullptr)
			{
				CDB_EditorError("Failed to load build targets from build engine");
				return;
			}

			//Cache the build targets (converting them to char*)
			OutputTypes.reserve(OT->size());
			std::transform(OT->begin(), OT->end(), std::back_inserter(OutputTypes), convert);
			if (OutputTypes.size() == 0)
			{
				CDB_EditorFatal("The current build engine must provide at least one output type");
			}
		}

		//Output the list of output types
		cout << "List of valid build targets:";
		for (int i = 0; i < OutputTypes.size(); ++i)
		{
			cout << "\n[" << i << "] " << OutputTypes[i];
		}
		cout << "\n\nEnter a build target index: ";
		int Target = std::stoi(GetInput());

		cout << "Enter output directory: ";
		std::string OutputDir = GetInput();

		Distribution = "";
		
		cout << "\nYou have selected the build target \"" << OutputTypes[Target] << "\". Project will be built to location \"" << OutputDir << "\"\n\nBuild project? [Y/n] ";

		std::string tmp = GetInput();
		if (!(IsNo(tmp)))
		{
			std::string OT = OutputTypes[Target];
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
				std::string Name = "�CadBerryAutogeneratedEntryPoint@" + proj->Name + "@" + CDB_VersionString;
				CDB::Application::Get().m_BuildEngine->Build(proj->Path, Name,
					proj->PreBuildDir, OutputDir, OT, Distribution);
			}
			else
			{
				CDB::Application::Get().m_BuildEngine->Build(proj->Path, proj->PathToEntryPoint,
					proj->PreBuildDir, OutputDir, OT, Distribution);
			}
		}
	}


	void WindowMenu()
	{
		using std::cout, std::cin, std::endl;

		cout <<
R"(----------------------------
     CadBerry Windows
----------------------------

List of windows:)";
		int ModuleNum = 0;
		for (Module* module : Application::Get().Modules)
		{
			std::string* s = module->GetViewportNames();
			bool CreateWindow = false;
			for (int i = 0; i < module->NumViewports; ++i)
			{
				cout << "\n[" << ModuleNum * Application::Get().Modules.size() + i << "] " << s[i];
			}
			++ModuleNum;
		}

		cout << "\n\nEnter the index of one of these windows to open that window or type \"exit\" to exit this menu" << endl;
		std::string Input = GetInput();
		if (Input == "exit")
			return;
		int InputNum = std::stoi(Input);

		//Get the module and viewport number
		for (int ModuleNum = 0; ModuleNum < Application::Get().Modules.size(); ++ModuleNum)
		{
			if (InputNum < Application::Get().Modules[ModuleNum]->NumViewports)
			{
				//Create the viewport
				Viewport* NewViewport = Application::Get().Modules[ModuleNum]->CreateViewport(
					Application::Get().Modules[ModuleNum]->GetViewportNames()[InputNum]);

				Application::Get().AddViewport(NewViewport);
				NewViewport->Start();
				break;
			}
			InputNum -= Application::Get().Modules[ModuleNum]->NumViewports;
		}
	}



	void ViewportLayer::AddViewport(Viewport* viewport)
	{
		OpenViewports.push_back(viewport);
	}

	Viewport::~Viewport()
	{

	}

	void Viewport::HeadlessInput()
	{
		std::cout << "The viewport \"" << this->Name << "\" doesn't support headless mode" << std::endl;
	}


	/*
	CadBerry's settings window. Here's all our settings:
	 - Project name
	 - Project precompilation directory
	 - Precompile files
	 - Project precompilation interval

	 Build settings:
	 - Should CadBerry maintain an entry point?
	 A little bit on entry points:
	 Instead of asking what file you want to use as an entry point, CadBerry will generate a gil file with the name "¬CadBerryAutogeneratedEntryPoint@ProjectName@CadBerryVersion". 
	 Because of the ¬ character, it's impossible to import this file from inside a GIL file

	 If no entry point:
	 - Path to entry point 
	 
	 If entry point:
	 - Target organism
	 - Entry sequences

	*/
	void ViewportLayer::SettingsWindow()
	{
		Project* proj = CDB::Application::Get().OpenProject.raw();

		ImGui::InputText("Project name", &proj->Name);
		ImGui::Spacing();
		ImGui::Spacing();

		//Precompilation
		ImGui::Checkbox("Precompile files", &proj->PrecompileFiles);

		if (proj->PrecompileFiles)
		{
			ImGui::InputFloat("Precompilation interval", &proj->PrecompilationInterval);
			ImGui::InputText("Precompilation directory", &proj->PreBuildDir);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Checkbox("Create entry point", &proj->MaintainEntryPoint);

		if (proj->MaintainEntryPoint)
		{
			ImGui::InputText("Target organism", &proj->TargetOrganism);
			ImGui::Spacing();
			InputStringVector("Sequences to compile", proj->EntrySequences, "sequence", "Sequence name");
		}
		else
		{
			ImGui::InputText("Relative path to project entry point", &proj->PathToEntryPoint);
		}

		proj->WriteToFile();
	}
}