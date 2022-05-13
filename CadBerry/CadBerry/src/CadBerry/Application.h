#pragma once
#include <cdbpch.h>
#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "CadBerry/LayerStack.h"
#include "CadBerry/ImGui/ImGuiLayer.h"
#include "CadBerry/Project/Project.h"
#include "CadBerry/Threading/ThreadPool.h"

#include "CadBerry/Module/Viewport.h"
#include "CadBerry/Module/Module.h"

#include "CadBerry/BuildEngine/BuildEngine.h"

#include "CadBerry/Rendering/Shader.h"
#include "CadBerry/Rendering/Buffer.h"
#include "CadBerry/Rendering/VertexArray.h"

#include "CadBerry/Utils/memory.h"

namespace CDB
{
	class CDBAPI Application
	{
	public:
		Application(bool headless = false);
		~Application();

		void GetProject();
		void StartEditor();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PrecompileFile(std::string Path, std::string PreBuildPath);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *EditorWindow; }
		inline void AddViewport(Viewport* viewport) { Viewports->AddViewport(viewport); }

		scoped_ptr<Project> OpenProject = nullptr;

		//The BuildEngine
		scoped_ptr<BuildEngine> m_BuildEngine;

		//Module stuff
		std::vector<Module*> Modules;

		std::string PathToEXE;
		int DirNameLength;
		std::string PreBuildDir;
		float dt = 0;

		//Because a WindowCloseEvent is blocking and we don't imediately exit the program when the window is closed, this bool allows the 
		//event to tell the program the window should close at the end of the current frame. 
		bool ShouldExit = false;
		bool NewProj = false;    //Should we check if the user already has a project?
		bool ShowBuildWindow = false;
		bool ShowPackages = false;

		//Is CadBerry being run in headless mode?
		bool Headless

		scoped_ptr<VertexArray> VertexArray;
	private:
		void Main();
		void CheckExit();
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool running;
		ImGuiLayer* GuiLayer;
		ViewportLayer* Viewports;
		ThreadPool* m_ThreadPool;

		static Application* s_Instance;

		Window* EditorWindow;

		//Both of these are owned by VertexArray, but a copy is "borrowed" by Application.h
		IndexBuffer* IndexBuffer;
		VertexBuffer* VertexBuffer;

		Shader* shader;
	};
}