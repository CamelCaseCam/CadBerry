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

#include "whereami.h"

#include <glad/glad.h>

namespace CDB
{
	Application* Application::s_Instance = nullptr;
	void Application::Main()
	{
		this->PreBuildDir = this->OpenProject->Path + this->OpenProject->PreBuildDir;


		EditorWindow = Window::Create();

		GuiLayer = new ImGuiLayer();    //CheckEnd will delete EditorWindow which will delete GuiLayer, so we're initializing a new one
		Viewports = new ViewportLayer();

		EditorWindow->m_LayerStack.PushOverlay(GuiLayer);
		EditorWindow->m_LayerStack.PushLayer(Viewports);
<<<<<<< HEAD
		glGenVertexArrays(1, &VertexArray);
=======
		/*glGenVertexArrays(1, &VertexArray);
>>>>>>> 6bb4493681bd7d96fea1812202eb7077be127cf8
		glBindVertexArray(VertexArray);

		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

		float vertices[3 * 3] = {
<<<<<<< HEAD
			0.0f, 0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f
=======
			-0.5f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
>>>>>>> 6bb4493681bd7d96fea1812202eb7077be127cf8
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
		
		unsigned int indices[3] = { 0, 1, 2 };
<<<<<<< HEAD
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		std::string VertSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			out vec3 OutPos;

			void main()
			{
				OutPos = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string FragSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 colour;
			in vec3 OutPos;
			vec3 C1 = vec3(1.0, 0.0, 0.0);
			vec3 C2 = vec3(0.0, 1.0, 0.0);

			void main()
			{
				colour = vec4(C1 * OutPos.x + (1.0 - C2 * OutPos.x), 1.0);
			}
		)";

		shader = new Shader(VertSrc, FragSrc);
=======
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/
>>>>>>> 6bb4493681bd7d96fea1812202eb7077be127cf8

		running = true;

		EditorWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		while (running)
		{
<<<<<<< HEAD
			shader->Bind();
			glBindVertexArray(VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
=======
			/*glBindVertexArray(VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);*/
>>>>>>> 6bb4493681bd7d96fea1812202eb7077be127cf8

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