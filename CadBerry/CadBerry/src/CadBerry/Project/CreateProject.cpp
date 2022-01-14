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
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Projects", NULL, flags);


        //The actual GUI
        for (ProjInfo& project : this->Projects)
        {
            if (ImGui::Selectable(project.Name.c_str()))
            {
                Application::Get().OpenProject = Project::ReadFromFile(project.Path);
                Application::Get().ShouldExit = true;
            }
        }
        if (ImGui::Button("New Project"))
            this->CreateProject = true;
        ImGui::SameLine(100);
        if (ImGui::Button("Load Project"))
        {
            nfdchar_t* FilePath;
            nfdresult_t Result = NFD_OpenDialog(NULL, NULL, &FilePath);

            if (Result == NFD_OKAY)
            {
                CDB::Application::Get().OpenProject = Project::ReadFromFile(std::string(FilePath));

                std::ofstream OutFile;
                if (std::filesystem::exists(Application::Get().PathToEXE + "/CDBProjectList.cfg"))
                {
                    OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg", std::ios_base::app);
                    OutFile << "\n";
                }
                else
                {
                    OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg");
                }
                OutFile << Application::Get().OpenProject->Name << ":" << Application::Get().OpenProject->Path << "/" 
                    << Application::Get().OpenProject->Name <<  ".berry";

                Application::Get().ShouldExit = true;
            }
        }

        if (CreateProject)
        {
            ImGui::Begin("Create project", &CreateProject);
            this->CreateNewProject();
            ImGui::End();
        }
        
        ImGui::End();
	}

    void ProjectCreationLayer::OnAttach()
    {
        ImGuiIO& io = ImGui::GetIO();    //We don't want viewports since it's just a project creation wizard
        io.ConfigFlags &= !ImGuiConfigFlags_ViewportsEnable;
    }

    void ProjectCreationLayer::CreateNewProject()
    {
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

            std::ofstream OutFile;
            if (std::filesystem::exists(Application::Get().PathToEXE + "/CDBProjectList.cfg"))
            {
                OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg", std::ios_base::app);
                OutFile << "\n";
            }
            else
            {
                OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg");
            }
            OutFile << Application::Get().OpenProject->Name << ":" << Application::Get().OpenProject->Path << "/"
                << Application::Get().OpenProject->Name << ".berry";

            Application::Get().ShouldExit = true;
        }
    }
}