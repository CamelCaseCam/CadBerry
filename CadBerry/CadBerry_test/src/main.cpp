#include "catch_test_macros.hpp"
#include "CadBerry.h"
#include "CadBerry/Platform/Headless/HeadlessInput.h"

#include "GIL.h"
#include "GIL/Lexer/Lexer.h"

#include <cstdio>

#include "GILExamples.h"

namespace CDB
{
	extern "C"
	{
		__declspec(dllimport) CDB::BuildEngine* __stdcall GetBuildEngine();
	}
	

	void DeleteProjectCache()
	{
		std::remove((CDB::Application::Get().PathToEXE + "/CDBLastProj.cfg").c_str());
		std::remove((CDB::Application::Get().PathToEXE + "/CDBProjectList.cfg").c_str());
	}


#pragma region CadBerryEditorTests


	TEST_CASE("set up CadBerry editor", "[setup]")
	{
		CDB::scoped_ptr<Application> MainApp = new Application(true);

		DeleteProjectCache();
		CurrentInputInVector = 0;

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "new project", "Example", "", ""};

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->ShouldExit = true;
		MainApp->StartEditor();

		//Cleanup code
		DeleteProjectCache();
		CurrentInputInVector = 0;
	}

	TEST_CASE("create CadBerry project", "[projects]")
	{
		CDB::scoped_ptr<Application> MainApp = new Application(true);

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "new project", "Example", CDB::Application::Get().PathToEXE + "/Tests/", ""};

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->ShouldExit = true;
		MainApp->StartEditor();

		DeleteProjectCache();
		CurrentInputInVector = 0;
	}

	TEST_CASE("load CadBerry project", "[projects]")
	{
		CDB::scoped_ptr<Application> MainApp = new Application(true);

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry", "exit" };

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->StartEditor();


		DeleteProjectCache();
		CurrentInputInVector = 0;
	}

	TEST_CASE("edit CadBerry project", "[projects]")
	{
		CDB::scoped_ptr<Application> MainApp = new Application(true);

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry", "project", "settings", 
			"y", "Example1", 
			"y", "10", "",
			"y", "S.Cerevisiae", "0",
			"exit" 
		};

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->StartEditor();

		//Make sure changes happened
		CHECK(MainApp->OpenProject->Name == "Example1");
		CHECK(MainApp->OpenProject->PrecompilationInterval == 10);
		CHECK(MainApp->OpenProject->PreBuildDir == "");
		CHECK(MainApp->OpenProject->TargetOrganism == "S.Cerevisiae");

		//We have to delete the app, we can do this by setting it to nullptr
		MainApp = nullptr;
		MainApp = new Application(true);

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		//Make sure changes persisted
		CHECK(MainApp->OpenProject->Name == "Example1");
		CHECK(MainApp->OpenProject->PrecompilationInterval == 10);
		CHECK(MainApp->OpenProject->PreBuildDir == "");
		CHECK(MainApp->OpenProject->TargetOrganism == "S.Cerevisiae");


		DeleteProjectCache();
		CurrentInputInVector = 0;
	}

	TEST_CASE("windows can be opened", "[modules]")
	{
		CDB_EditorInfo("Worked");
		scoped_ptr<Application> MainApp = new Application(true);

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry" };

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		CurrentInputInVector = 0;

		SECTION("one window can be opened and closed")
		{
			Inputs = { "windows", "0", "window list", "exit" };    //Open DNA Editor
			MainApp->StartEditor();

			REQUIRE(MainApp->GetViewportLayer()->OpenViewports.size() == 0);    //Windows should be deleted by editor exiting
		}

		SECTION("multiple windows can be opened")
		{
			Inputs = { "windows", "0", "windows", "1", "window list", "exit" };    //Open DNA Editor
			MainApp->StartEditor();

			REQUIRE(MainApp->GetViewportLayer()->OpenViewports.size() == 0);
		}

		SECTION("multiple copies of the same window can be opened")
		{
			Inputs = { "windows", "0", "windows", "0", "window list", "exit" };    //Open DNA Editor
			MainApp->StartEditor();

			REQUIRE(MainApp->GetViewportLayer()->OpenViewports.size() == 0);
		}

		DeleteProjectCache();
		CurrentInputInVector = 0;
	}


#pragma endregion CadBerryEditorTests

#pragma region GILTests

	TEST_CASE("build GIL files")
	{
		scoped_ptr<Application> MainApp = new Application(true);
		DeleteProjectCache();
		CurrentInputInVector = 0;

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry" };

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		auto engine = GetBuildEngine();

#define SaveAndLoad(project)
#include "GILTests.h"
		
	}
	
	TEST_CASE("Save and load GIL project files")
	{
		scoped_ptr<Application> MainApp = new Application(true);
		DeleteProjectCache();
		CurrentInputInVector = 0;

		REQUIRE(MainApp.raw() != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry" };

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		auto engine = GetBuildEngine();

#define SaveAndLoad(project) project->Save(Application::Get().PathToEXE + "/Tests/Example.cgil"); project = GIL::Parser::Project::Load(Application::Get().PathToEXE + "/Tests/Example.cgil")
#include "GILTests.h"

	}

#pragma endregion GILTests

	//TODO: look at this

	/*
	Testing notes:

	*/
}