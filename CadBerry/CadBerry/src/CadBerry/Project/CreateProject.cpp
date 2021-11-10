#include "cdbpch.h"
#include "CreateProject.h"
#include "imgui.h"
#include "CadBerry/Log.h"
#include "CadBerry/Application.h"

#include <nfd.h>

namespace CDB
{
	void ProjectCreationLayer::OnImGuiRender()
	{
        //We don't want this to be a seperate window, so we'll set these flags and disable viewports
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoTitleBar;
        flags |= ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoDocking;

        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Create a Project", NULL, flags);

        //The actual GUI
        ImGui::InputText("Project Name", ProjectName, IM_ARRAYSIZE(ProjectName));
        bool GetPath = ImGui::Button("Select project path");

        if (GetPath)
        {
            nfdresult_t result = NFD_PickFolder("", &Path);
        }

        bool CreateProject = ImGui::Button("Create Project!");
        if (CreateProject)
        {
            CDB_BuildInfo("Creating project {0} at location {1}", ProjectName, Path);
            Application::Get().OpenProject = new Project(ProjectName, Path);
            Application::Get().ShouldExit = true;
        }
        
        ImGui::End();
	}

    void ProjectCreationLayer::OnAttach()
    {
        ImGuiIO& io = ImGui::GetIO();    //We don't want viewports since it's just a project creation wizard
        io.ConfigFlags &= !ImGuiConfigFlags_ViewportsEnable;
    }
}