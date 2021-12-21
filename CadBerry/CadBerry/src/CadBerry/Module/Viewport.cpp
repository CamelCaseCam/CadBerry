#include <cdbpch.h>
#include "Viewport.h"
#include "CadBerry/Log.h"
#include "CadBerry/Module/Module.h"
#include "CadBerry/Application.h"
#include "CadBerry/BuildEngine/BuildDialog.h"

#include <GLFW/glfw3.h>

#include "imgui.h"

namespace CDB
{
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
				bool OpenSettings = false;
				bool BuildProject = false;
				ImGui::MenuItem("New Project", NULL, &NewProject);
				if (NewProject)
				{
					Application::Get().NewProj = true;
					Application::Get().ShouldExit = true;    //this will make us exit the editor window and open a project creation window
				}
				ImGui::MenuItem("Settings", NULL, &OpenSettings);
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

		Viewport* ToBeDeleted = nullptr;
		for (Viewport* viewport : OpenViewports)
		{
			viewport->Update(io.DeltaTime);
			if (viewport->Background) continue;

			ImGui::Begin(viewport->Name.c_str(), &viewport->IsOpen);
			viewport->GUIDraw();
			ImGui::End();
			if (!viewport->IsOpen)    //If the user has closed the viewport, mark that viewport to be deleted. 
			{
				ToBeDeleted = viewport;    //Since this is called every frame, it's impossible for the user to close more than one viewport. 
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
			delete vp;
	}

	void ViewportLayer::AddViewport(Viewport* viewport)
	{
		OpenViewports.push_back(viewport);
	}

	Viewport::~Viewport()
	{

	}
}