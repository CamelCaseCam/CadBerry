#define FMT_HEADER_ONLY
#include "CadBerry.h"
#include "CadBerry/Project/Project.h"
#include "CadBerry/Threading/ThreadPool.h"
#include "CadBerry/ExecuteCommand.h"

#include <filesystem>
#include <stdlib.h>
#include <sys/stat.h>

//Function definitions
void* PrecompileFilesTP(void* Params);
void PrecompileFiles(std::string Subdir);
bool IsChanged(std::string RelativePath);

#define MODULE_API
#ifdef CDB_PLATFORM_WINDOWS
#define MODULE_API __declspec(dllexport)
#endif



//TODO: Make it so that this is editable via the project settings
const float PrecompilationInterval = 10.0f;
std::string FullPrebuildPath;
CDB::Application* CDBApp;


class MODULE_API VSCodeWindow : public CDB::Viewport
{
public:
	VSCodeWindow() : CDB::Viewport("VSCode Window") {};
	~VSCodeWindow() override {};

	virtual void Start() override
	{
		CDBApp = &CDB::Application::Get();
		CDB::Project* OpenProject = CDBApp->OpenProject.raw();    //If this returns nullptr, something is seriously broken with the core editor
		#ifdef CDB_PLATFORM_WINDOWS
		CDB::ExecCommand("cmd.exe", "/C code \"" + OpenProject->Path + "\"")
		#else
		CDB::ExecCommand("code", "\"" + OpenProject->Path + "\"");
		#endif

		this->Background = true;

		if (s_Instance != nullptr)
		{
			delete s_Instance;
		}
		s_Instance = this;

		FullPrebuildPath = CDBApp->OpenProject->Path + CDBApp->OpenProject->PreBuildDir;
	}

	VSCodeWindow* s_Instance = nullptr;    //Because we don't know when/if the user closes vscode, we'll delete any duplicates of this object

	float CurrentTime = 0;
	virtual void Update(float dt) override
	{
		CurrentTime += dt;
		if (CurrentTime > PrecompilationInterval)
		{
			CurrentTime = 0;
			CDB_EditorInfo("Precompiling files");
			CDB::ThreadPool::Get()->AddStandardTask(PrecompileFilesTP, nullptr);
		}
	}
};

void* PrecompileFilesTP(void* Params)
{
	PrecompileFiles("");
	return nullptr;
}

void PrecompileFiles(std::string Subdir)
{
	const std::filesystem::directory_iterator end{};
	std::filesystem::directory_iterator CurrentFolder{ std::filesystem::path(CDBApp->OpenProject->Path).append(Subdir) };
	for (CurrentFolder; CurrentFolder != end; ++CurrentFolder)
	{
		if(std::filesystem::is_regular_file(*CurrentFolder))
		{
			if (CurrentFolder->path().extension().string() == ".gil")
			{
				if (IsChanged(std::filesystem::relative(CDBApp->OpenProject->Path, std::filesystem::path(CDBApp->OpenProject->Path).append(Subdir)).string()))
				{
					//TODO: This could cause problems if precompilation takes too long, as the user might modify the gil file before the old version finishes compiling
					CDBApp->PrecompileFile(CurrentFolder->path().string(), FullPrebuildPath + "\\" + CurrentFolder->path().stem().string() + ".cgil");
				}
			}
		}
		else if (std::filesystem::is_directory(*CurrentFolder) && CurrentFolder->path().string() + "\\" != FullPrebuildPath)
		{
			PrecompileFiles(CurrentFolder->path().string());
		}
	}
}

bool IsChanged(std::string RelativePath)
{
	//HACK: I need to make this portable
	std::filesystem::path Path = CDBApp->OpenProject->Path + "\\" + RelativePath;    //Get the last edit time of the gil file
	auto LWTime = std::filesystem::last_write_time(Path);

	Path = FullPrebuildPath + "\\" + RelativePath + ".cgil";
	if (std::filesystem::exists(Path))    //Check if the CGIL file exists
	{
		auto LPCTime = std::filesystem::last_write_time(Path);    //If it does exist, check if the gil file was modified after the CGIL file
		return LWTime > LPCTime;
	}
	return true;
}

class MODULE_API VSCodeIntegrationModule : public CDB::Module
{
public:
	VSCodeIntegrationModule() : CDB::Module("VSCode integration module") {}
	~VSCodeIntegrationModule() override {}
	
	std::string Viewports[1] = { "Code Editor" };
	virtual std::string* GetViewportNames() override
	{
		NumViewports = 1;
		return Viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string viewport) override
	{
		return new VSCodeWindow();
	}
};

//Expose the module to CadBerry
extern "C"
{
#ifdef CDB_PLATFORM_WINDOWS
	__declspec(dllexport) CDB::Module* __stdcall GetModule()
	{
		return new VSCodeIntegrationModule();
	}
#else
	CDB::Module* GetModule()
	{
		return new VSCodeIntegrationModule();
	}
#endif
}