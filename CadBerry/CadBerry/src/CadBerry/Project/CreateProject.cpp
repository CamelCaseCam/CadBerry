#include "cdbpch.h"
#include "CreateProject.h"

#pragma warning(push, 0)
#include "imgui.h"
#pragma warning(pop)

#include "CadBerry/Log.h"
#include "CadBerry/Application.h"
#include "CadBerry/Platform/Headless/HeadlessInput.h"

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
        if (!Application::Get().Headless)
        {
            ImGuiIO& io = ImGui::GetIO();    //We don't want viewports since it's just a project creation wizard
            io.ConfigFlags &= !ImGuiConfigFlags_ViewportsEnable;
        }
    }

    void ProjectCreationHeadless();
    void ProjectCreationLayer::HeadlessInput()
    {
        using std::cout, std::cin, std::endl;
        cout << 
R"(----------------------------
          Projects
----------------------------
Type "new project" to create a new project
Type "load project" to load a project from the disk
Type the index of an existing project to load that project

List of existing projects:)";

        for (int i = 0; i < this->Projects.size(); ++i)
        {
            cout << "\n[" << i << "] " << this->Projects[i].Name;
        }
        cout << "\n\nEnter a command: ";
        std::string Command = GetInput();
        if (Command == "new project")
        {
            ProjectCreationHeadless();
        }
        else if (Command == "load project")
        {
            cout << "Enter project path: ";
            std::string FilePath = GetInput();

            if (FilePath != "")
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
                OutFile << Application::Get().OpenProject->Name << ":" << Application::Get().OpenProject->PathToBerryFile;

                Application::Get().ShouldExit = true;
            }
        }
        else
        {
            Application::Get().OpenProject = Project::ReadFromFile(this->Projects[std::stoi(Command)].Path);
            Application::Get().ShouldExit = true;
        }
    }

    void ProjectCreationHeadless()
    {
        using std::cout, std::cin, std::endl;
        cout <<
R"(----------------------------
   Project Creation Wizard
----------------------------
Enter project name: )";
        std::string ProjectName = GetInput();
        cout << "Enter project path: ";
        std::string Path = GetInput();

        CDB_BuildInfo("Creating project {0} at location {1}", ProjectName, Path);
        Application::Get().OpenProject = new Project(ProjectName, Path);

        std::ofstream OutFile;

        //If the list of projects exists, we want to add our project to it
        if (std::filesystem::exists(Application::Get().PathToEXE + "/CDBProjectList.cfg"))
        {
            OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg", std::ios_base::app);
            OutFile << "\n";
        }
        else
        {
            OutFile.open(Application::Get().PathToEXE + "/CDBProjectList.cfg");
        }
        OutFile << Application::Get().OpenProject->Name << ":" << Application::Get().OpenProject->PathToBerryFile;

        Application::Get().ShouldExit = true;
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
            if (Path != nullptr)
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
                OutFile << Application::Get().OpenProject->Name << ":" << Application::Get().OpenProject->PathToBerryFile;

                Application::Get().ShouldExit = true;
            }
            else
            {
				CDB_EditorError("You must select a path to create the project");
            }
        }
    }
}