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

#include "whereami.h"

#include <glad/glad.h>

namespace CDB
{
	void FetchPackages() { GetPackages(Application::Get().PathToEXE); }

	Application* Application::s_Instance = nullptr;
	void Application::Main()
	{
		this->PreBuildDir = this->OpenProject->Path + this->OpenProject->PreBuildDir;

		this->m_ThreadPool->AddStandardTask(FetchPackages);

		EditorWindow = Window::Create();

		GuiLayer = new ImGuiLayer();    //CheckEnd will delete EditorWindow which will delete GuiLayer, so we're initializing a new one
		Viewports = new ViewportLayer();

		EditorWindow->m_LayerStack.PushOverlay(GuiLayer);
		EditorWindow->m_LayerStack.PushLayer(Viewports);

		running = true;

		EditorWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		while (running)
		{

			EditorWindow->OnUpdate();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnUpdate();

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
		EditorWindow = Window::Create(WindowProps("New Project", 480U, 720U));
		EditorWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		EditorWindow->m_LayerStack.PushOverlay(GuiLayer);
		EditorWindow->m_LayerStack.PushLayer(new ProjectCreationLayer());
		running = true;

		while (running)
		{
			EditorWindow->OnUpdate();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnUpdate();

			GuiLayer->Begin();
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnImGuiRender();
			GuiLayer->End();

			CheckExit();
		}
		if (OpenProject == nullptr)
		{
			CDB_EditorError("You must open or create a project");
			GuiLayer = new ImGuiLayer();
			GetProject();
		}
		this->OpenProject->WriteToFile();

		std::ofstream OutputCurrentProject;
		OutputCurrentProject.open(this->PathToEXE + "/CDBLastProj.cfg", std::ios::out);
		std::string path = OpenProject->Path + "\\" + OpenProject->Name + ".berry";
		OutputCurrentProject << path;
		OutputCurrentProject.close();

		return;
	}

	void Application::StartEditor()
	{
		CDB_EditorInfo("Starting CadBerry...");
		Main();
	}

	typedef Module* (__stdcall* f_GetModule)();
	typedef BuildEngine* (__stdcall* f_GetBuildEngine)();
	Application::Application()
	{
		Log::Init();
		CDB_EditorAssert(!s_Instance, "Application already exists")
		s_Instance = this;
		m_ThreadPool = ThreadPool::Get();
		GuiLayer = new ImGuiLayer();

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

		std::filesystem::directory_iterator ModuleFolder{ std::filesystem::path(PathToEXE).append("Modules\\")};
		for (ModuleFolder; ModuleFolder != end; ++ModuleFolder)
		{
			if (std::filesystem::is_regular_file(*ModuleFolder))
			{
				//check if it's a dll
				auto CPath = ModuleFolder->path().c_str();
				std::string Path = ModuleFolder->path().string();
				if (CPath[Path.length() - 1] != 'l')    //If the last char is l, then we know it's probably a dll. Otherwise, we'll continue
				{
					continue;
				}
				//Load the dll
				HINSTANCE DLLID = LoadLibrary(CPath);

				if (!DLLID)
				{
					CDB_EditorError("Could not load dll \"{0}\"", Path);
					continue;
				}

				//Get the "GetModule" function
				f_GetModule GetModule = (f_GetModule)GetProcAddress(DLLID, "GetModule");
				if (!GetModule)
				{
					CDB_EditorError("Could not locate GetModule function in dll \"{0}\"", Path);
					FreeLibrary(DLLID);
					continue;
				}
				Modules.push_back(GetModule());
			}
		}

		//Load build engine
		std::filesystem::directory_iterator BuildEngineFolder{ std::filesystem::path(PathToEXE).append("Build\\") };
		for (BuildEngineFolder; BuildEngineFolder != end; ++BuildEngineFolder)
		{
			if (std::filesystem::is_regular_file(*BuildEngineFolder))
			{
				//check if it's a dll
				auto CPath = BuildEngineFolder->path().c_str();
				std::string Path = BuildEngineFolder->path().string();
				if (CPath[Path.length() - 1] != 'l')    //If the last char is l, then we know it's probably a dll. Otherwise, we'll continue
				{
					continue;
				}
				//Load the dll
				HINSTANCE DLLID = LoadLibrary(CPath);

				if (!DLLID)
				{
					CDB_EditorError("Could not load dll \"{0}\"", Path);
					continue;
				}

				//Get the "GetModule" function
				f_GetBuildEngine GetBuildEngine = (f_GetBuildEngine)GetProcAddress(DLLID, "GetBuildEngine");
				if (!GetBuildEngine)
				{
					CDB_EditorError("Could not locate GetModule function in dll \"{0}\"", Path);
					FreeLibrary(DLLID);
					continue;
				}
				m_BuildEngine = GetBuildEngine();

				break;
			}
		}
	}

	Application::~Application()
	{
		//GuiLayer will have already been deleted
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
			for (Layer* layer : EditorWindow->m_LayerStack)
				layer->OnDetach();
			delete EditorWindow;
			running = false;
			ShouldExit = false;
			if (NewProj)
			{
				GuiLayer = new ImGuiLayer();
				GetProject();
				GuiLayer = new ImGuiLayer();
				Main();
			}
		}
	}
}