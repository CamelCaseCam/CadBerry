#include "cdbpch.h"
#include "Application.h"

#include "Log.h"
#include "Events/Event.h"
#include "CadBerry/ImGui/ImGuiLayer.h"
#include "CadBerry/Input.h"
#include "CadBerry/Project/CreateProject.h"
#include "CadBerry/Module/Viewport.h"
#include "CadBerry/BuildEngine/BuildEngine.h"
#include "CadBerry/Tests/SimpleViewport.h"
#include "CadBerry/BuildEngine/BuildDialog.h"
#include "CadBerry/Packages/LoadPackages.h"
#include "CadBerry/Packages/Update.h"
#include "Math/Equation.h"
#include "DNAVis/DisplayDNA.h"
#include "SharedLib.h"
#include "Core.h"

#include "Utils/RNAFuncs.h"

#include "RNA.h"

#include "whereami.h"

#include <glad/glad.h>

namespace CDB
{
	void* FetchPackages(void* NoParams) { GetPackages(Application::Get().PathToEXE); return nullptr; }

	Application* Application::s_Instance = nullptr;

	void Application::Main()
	{

		this->PreBuildDir = this->OpenProject->Path + this->OpenProject->PreBuildDir;

		this->m_ThreadPool->AddStandardTask(FetchPackages, nullptr);

		if (Headless)
		{
			EditorWindow = Window::CreateHeadless(WindowProps("CadBerry Editor - " + OpenProject->Name));
		}
		else
		{
			EditorWindow = Window::Create(WindowProps("CadBerry Editor - " + OpenProject->Name));

			GuiLayer = new ImGuiLayer();    //CheckEnd will delete EditorWindow which will delete GuiLayer, so we're initializing a new one
			EditorWindow->m_LayerStack.PushOverlay(GuiLayer);
		}

		Viewports = new ViewportLayer();

		EditorWindow->m_LayerStack.PushLayer(Viewports);

		running = true;

		EditorWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		while (running)
		{

			EditorWindow->OnUpdate();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnUpdate();

			if (!this->Headless)
			{
				GuiLayer->Begin();
				for (Layer* layer : EditorWindow->m_LayerStack)
					layer->OnImGuiRender();

				if (ShowBuildWindow)
				{
					ImGui::Begin("Build project", &ShowBuildWindow, ImGuiWindowFlags_NoDocking);
					BuildDialog();
					ImGui::End();
				}
				if (ShowPackages)
				{
					ImGui::Begin("Packages", &ShowPackages, ImGuiWindowFlags_NoDocking);
					ShowPackageManager();
					ImGui::End();
				}
				GuiLayer->End();
			}

			CheckExit();
		}

		//Viewports is deleted by LayerStack when the window is deleted, so we don't have to worry about deleting it now
	}

	//Function to load projects from disk or create a new one. Right now, it automatically creates a new one. 
	void Application::GetProject()
	{
		//If there's a config file with the path to the last project, open the last project
		std::ifstream LProj;
		LProj.open(this->PathToEXE + "/CDBLastProj.cfg", std::ios::in);
		if (!NewProj && LProj.is_open())
		{
			std::string line;
			std::getline(LProj, line);
			this->OpenProject = Project::ReadFromFile(line);
			LProj.close();
			CDB_EditorInfo("Worked");
			return;
		}
		NewProj = false;
		LProj.close();

		std::ifstream ProjIn;
		std::vector<ProjInfo> Projects;
		ProjIn.open(this->PathToEXE + "/CDBProjectList.cfg", std::ios::in);

		std::string ProjName;
		while (std::getline(ProjIn, ProjName))
		{
			int ColonIdx = ProjName.find(":");
			ProjInfo proj;
			proj.Name = ProjName.substr(0, ColonIdx);
			proj.Path = ProjName.substr(ColonIdx + 1);
			Projects.push_back(proj);
		}
		ProjIn.close();

		if (Headless)
			EditorWindow = Window::CreateHeadless(WindowProps("New Project", 480U, 720U));
		else
			EditorWindow = Window::Create(WindowProps("New Project", 480U, 720U));


		EditorWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		if (!Headless)
			EditorWindow->m_LayerStack.PushOverlay(GuiLayer);

		EditorWindow->m_LayerStack.PushLayer(new ProjectCreationLayer(Projects));
		running = true;

		while (running)
		{
			EditorWindow->OnUpdate();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnUpdate();

			if (!Headless)
			{
				GuiLayer->Begin();
				for (Layer* layer : EditorWindow->m_LayerStack)
					layer->OnImGuiRender();
				GuiLayer->End();
			}

			CheckExit();
		}
		if (OpenProject == nullptr)
		{
			CDB_EditorError("You must open or create a project");
			if (!Headless)
			{
				GuiLayer = new ImGuiLayer();
			}
			GetProject();
		}
		this->OpenProject->WriteToFile();

		std::ofstream OutputCurrentProject;
		OutputCurrentProject.open(this->PathToEXE + "/CDBLastProj.cfg", std::ios::out);
		OutputCurrentProject << this->OpenProject->PathToBerryFile;
		OutputCurrentProject.close();

		return;
	}

	void Application::StartEditor()
	{
		CDB_EditorInfo("Starting CadBerry...");
		Main();
	}

#ifdef CDB_PLATFORM_WINDOWS
	#define SharedLibLastChar 'l'
#else 
	#define SharedLibLastChar 'o'
#endif

	typedef Module* (CDB_MODULE_FUNC* f_GetModule)();
	typedef BuildEngine* (CDB_MODULE_FUNC* f_GetBuildEngine)();
	Application::Application(bool headless)
	{
		ShouldExit = false;
		Log::Init();
		CDB_BuildInfo((void*)s_Instance);
		CDB_EditorAssert(!s_Instance, "Application already exists")
		s_Instance = this;
		this->Headless = headless;
		m_ThreadPool = ThreadPool::Get();

		if (!headless)
		{
			GuiLayer = new ImGuiLayer();
		}

		//load modules
		const std::filesystem::directory_iterator end{};

		{    //Creating a scope so DirName will automatically be deleted
			int len;
			DirNameLength = wai_getExecutablePath(NULL, 0, &len);    //Get path length
			char* DirName = new char[DirNameLength + 1];
			wai_getExecutablePath(DirName, DirNameLength, &len);    //Get path
			DirName[len] = '\0';    //Add nul termination char to path
			PathToEXE = DirName;    //Convert path to string
		}
		RNAContext::InitRNAContext(PathToEXE + "\\Data\\data_tables\\");

		#ifdef CDB_PLATFORM_WINDOWS
			SetDllDirectory(std::filesystem::path(PathToEXE).append("Build\\").c_str());
		#endif
		std::filesystem::directory_iterator ModuleFolder{ std::filesystem::path(PathToEXE).append("Modules")};
		for (ModuleFolder; ModuleFolder != end; ++ModuleFolder)
		{
			if (std::filesystem::is_regular_file(*ModuleFolder))
			{
				//check if it's a dll
				auto CPath = ModuleFolder->path().c_str();
				std::string Path = ModuleFolder->path().string();

				//TODO: Make this not scuffed
				if (CPath[Path.length() - 1] != SharedLibLastChar)    //If the last char is l, then we know it's probably a dll. Otherwise, we'll continue
				{
					continue;
				}

				//Get the "GetModule" function
				f_GetModule GetModule = GetSharedLibFunc<f_GetModule>(CPath, "GetModule");
				Modules.push_back(GetModule());
			}
		}

		//Load build engine
		std::filesystem::directory_iterator BuildEngineFolder{ std::filesystem::path(PathToEXE).append("Build") };
		for (BuildEngineFolder; BuildEngineFolder != end; ++BuildEngineFolder)
		{
			if (std::filesystem::is_regular_file(*BuildEngineFolder))
			{
				//check if it's a dll
				auto CPath = BuildEngineFolder->path().c_str();
				std::string Path = BuildEngineFolder->path().string();
				if (CPath[Path.length() - 1] != SharedLibLastChar)    //If the last char is l, then we know it's probably a dll. Otherwise, we'll continue
				{
					continue;
				}

				//Get the "GetBuildEngine" function
				f_GetBuildEngine GetBuildEngine = GetSharedLibFunc<f_GetBuildEngine>(CPath, "GetBuildEngine");
				m_BuildEngine = GetBuildEngine();

				break;
			}
		}
	}

	Application::~Application()
	{
		//GuiLayer will have already been deleted

		//Cleanup
		s_Instance = NULL;
		CDB_BuildInfo((void*)s_Instance);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowCloseEvent));

		if (running)
		{
			for (auto it = EditorWindow->m_LayerStack.end(); it != EditorWindow->m_LayerStack.begin();)
			{
				(*--it)->OnEvent(e);
				if (e.m_Handled)
					break;
			}
		}
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		ShouldExit = true;    //We need to wait for the current frame to end before exiting

		return true;
	}

	void Application::PushLayer(Layer* layer)
	{
		EditorWindow->m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		EditorWindow->m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PrecompileFile(std::string Path, std::string PreBuildPath)
	{
		m_BuildEngine->PreBuild(Path, PreBuildPath);
	}

	void Application::CheckExit()
	{
		if (ShouldExit)
		{
			OpenProject->WriteToFile();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnDetach();
			delete EditorWindow;
			running = false;
			ShouldExit = false;
			if (NewProj)
			{
				if (!Headless)
				{
					GuiLayer = new ImGuiLayer();
				}
				GetProject();

				if (!Headless)
				{
					GuiLayer = new ImGuiLayer();
				}
				Main();
			}
		}
	}
}