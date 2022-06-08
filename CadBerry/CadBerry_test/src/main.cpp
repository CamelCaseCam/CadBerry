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
		Application* MainApp = new Application(true);

		DeleteProjectCache();
		CurrentInputInVector = 0;

		REQUIRE(MainApp != nullptr);

		Inputs = { "new project", "Example", "", ""};

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->ShouldExit = true;
		MainApp->StartEditor();

		//Cleanup code
		DeleteProjectCache();
		CurrentInputInVector = 0;
		delete MainApp;
	}

	TEST_CASE("create CadBerry project", "[projects]")
	{
		Application* MainApp = new Application(true);

		REQUIRE(MainApp != nullptr);

		Inputs = { "new project", "Example", CDB::Application::Get().PathToEXE + "/Tests/", ""};

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->ShouldExit = true;
		MainApp->StartEditor();

		DeleteProjectCache();
		CurrentInputInVector = 0;
		delete MainApp;
	}

	TEST_CASE("load CadBerry project", "[projects]")
	{
		Application* MainApp = new Application(true);

		REQUIRE(MainApp != nullptr);

		Inputs = { "load project", Application::Get().PathToEXE + "/Tests/Example.berry", "exit" };

		//Load project
		MainApp->GetProject();
		REQUIRE(MainApp->OpenProject.raw() != nullptr);

		MainApp->StartEditor();


		DeleteProjectCache();
		CurrentInputInVector = 0;
		delete MainApp;
	}

	TEST_CASE("edit CadBerry project", "[projects]")
	{
		Application* MainApp = new Application(true);

		REQUIRE(MainApp != nullptr);

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

		delete MainApp;

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
		delete MainApp;
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

		SECTION("Setting target organism")
		{
			auto tokens = GIL::Lexer::Tokenize(TargetExample1);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "S.Cerevisiae");

			tokens = GIL::Lexer::Tokenize(TargetExample2);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "E.coli");

			tokens = GIL::Lexer::Tokenize(TargetExample3);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Escherichia coli");

			tokens = GIL::Lexer::Tokenize(TargetExample4);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "human");

			tokens = GIL::Lexer::Tokenize(TargetExample5);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Pichia");

			tokens = GIL::Lexer::Tokenize(TargetExample6);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Pichia Pastoris");

			tokens = GIL::Lexer::Tokenize(TargetExample7);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Saccharomyces cerevisiae");

			tokens = GIL::Lexer::Tokenize(TargetExample8);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Unoptimized");

			tokens = GIL::Lexer::Tokenize(TargetExample9);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "Yeast");

			//Make sure it doesn't parse invalid targets
			tokens = GIL::Lexer::Tokenize(TargetExampleError);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->TargetOrganism == "ERR_NO_TARGET");
		}

		SECTION("Preprocessor commands")
		{
			//Regions
			auto tokens = GIL::Lexer::Tokenize(RegionExample);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			auto Output = GIL::Compiler::Compile(project);
			REQUIRE(Output.first.size() == 2);

			CHECK(Output.first[0].Name == "Region2");
			CHECK(Output.first[1].Name == "Region1");

			CHECK(Output.first[1].Start  == 0 );
			CHECK(Output.first[1].End    == 12 );
			CHECK(Output.first[0].Start  == 6 );
			CHECK(Output.first[0].End    == 12);



			tokens = GIL::Lexer::Tokenize(OverlappingRegionExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			REQUIRE(Output.first.size() == 2);

			CHECK(Output.first[0].Name == "Region1");
			CHECK(Output.first[1].Name == "Region2");

			CHECK(Output.first[0].Start == 0 );
			CHECK(Output.first[0].End   == 12);
			CHECK(Output.first[1].Start == 6 );
			CHECK(Output.first[1].End   == 18);

			
			//SetAttr
			tokens = GIL::Lexer::Tokenize(SetAttrExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);

			REQUIRE(project->Attributes.size() > 0);

			CHECK(project->Attributes["a normal string"] == "another completely normal string");
			CHECK(project->Attributes["A More Complicated STRING #%$##@$%RWETRBFIKUOI::P}{(*"] == "~~~~~~~~~#%$#^$%^&YHGF::");


			//#Var
			tokens = GIL::Lexer::Tokenize(CreateVarExample1);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->StrVars.size() == 1);
			CHECK(project->NumVars.size() == 1);

			CHECK(project->StrVars["Var1"] == "SomeString");
			CHECK(project->NumVars["Var2"] == 10);


			tokens = GIL::Lexer::Tokenize(CreateVarExample2);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->StrVars.size() == 1);
			CHECK(project->NumVars.size() == 1);

			CHECK(project->StrVars["SSDGSFDV  SDFSEFDCSR##@23or3fnreiuh34"] == "TestValue");
			CHECK(project->NumVars["THIS IS A NUMBER23"] == 3892302332.6);


			//#Inc
			tokens = GIL::Lexer::Tokenize(IncExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(project->StrVars.size() == 0);
			CHECK(project->NumVars.size() == 1);

			CHECK(project->NumVars["TestVar"] == 22234.0);

			
			//#Dec
			tokens = GIL::Lexer::Tokenize(DecExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(project->StrVars.size() == 0);
			CHECK(project->NumVars.size() == 1);

			CHECK(project->NumVars["TestVar"] == 22232.0);


			//#If
			tokens = GIL::Lexer::Tokenize(PreproIfExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(project->StrVars.size() == 0);
			CHECK(project->NumVars.size() == 3);

			CHECK(project->NumVars["Counter"] == 1.0);


			//#Else
			tokens = GIL::Lexer::Tokenize(PreproElseExample);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(project->StrVars.size() == 0);
			CHECK(project->NumVars.size() == 3);

			CHECK(project->NumVars["Counter"] == 1.0);
		}

		SECTION("DNA and amino acid literals")
		{
			auto tokens = GIL::Lexer::Tokenize(DNAExample1);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			auto Output = GIL::Compiler::Compile(project);
			REQUIRE(Output.second == "AAATTTCCCGGG");

			tokens = GIL::Lexer::Tokenize(DNAExample2);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			REQUIRE(Output.second == "DSSDGFSSEVREFEWADSFVTERSD");

			tokens = GIL::Lexer::Tokenize(DNAExample3);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			REQUIRE(Output.second == "@#@#454356w5r23$#$@453425");


			tokens = GIL::Lexer::Tokenize(AminoSequenceExample1);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			for (auto& c : Output.second) c = std::toupper(c);
			REQUIRE(Output.second == "GGTATTCTTTAAGCTGTTATGCCTTTTTGGAGTACTAATCAATATTGTAAACGTCATGATGAA");

			tokens = GIL::Lexer::Tokenize(AminoSequenceExample2);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			for (auto& c : Output.second) c = std::toupper(c);
			REQUIRE(Output.second == "GGTATTTTGTAAGCTGTTATGCCATTTTGGTCTACTAATCAATATTGTAAAAGACATGATGAA");
		}

		SECTION("Sequences and operations")
		{
			auto tokens = GIL::Lexer::Tokenize(SequenceNoParams);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			auto Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "CCCAAATTTGGG");

			tokens = GIL::Lexer::Tokenize(MultiSequenceNoParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "AAATTTCCC");

			tokens = GIL::Lexer::Tokenize(SequenceWithParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTAAACCC");

			tokens = GIL::Lexer::Tokenize(SequenceWithParamsWithParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTAAAAAACCC");

			//Typed params 
			tokens = GIL::Lexer::Tokenize(SequenceWithTypedParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTAAACCC");

			tokens = GIL::Lexer::Tokenize(SequenceWithTypedParamsWithParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTAAAAAACCC");

			tokens = GIL::Lexer::Tokenize(CustomType);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->Types[project->TypeName2Idx["TYPE"]] != nullptr);

			tokens = GIL::Lexer::Tokenize(CustomTypeInheritance);
			project = GIL::Parser::Project::Parse(*tokens);
			CHECK(project->Types[project->TypeName2Idx["TYPE"]]->IsOfType(project->Types[project->TypeName2Idx["cds"]], false));


			//Operations
			tokens = GIL::Lexer::Tokenize(OpNoParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTCCCTTT");

			tokens = GIL::Lexer::Tokenize(SequenceWithInnerCode);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTCCCTTT");

			tokens = GIL::Lexer::Tokenize(OpSequenceParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "TTTCCCGGGTTT");
		}

		SECTION("Forwards")
		{
			auto tokens = GIL::Lexer::Tokenize(SequenceForward);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			auto Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "AAAAA");

			tokens = GIL::Lexer::Tokenize(SequenceForwardWithParams);
			project = GIL::Parser::Project::Parse(*tokens);
			Output = GIL::Compiler::Compile(project);
			CHECK(Output.second == "AAAAATTT");
		}

		SECTION("For and from")
		{
			auto tokens = GIL::Lexer::Tokenize(ForExample);
			GIL::Parser::Project* project = GIL::Parser::Project::Parse(*tokens);
			auto Output = GIL::Compiler::Compile(project);
			for (auto& c : Output.second) c = std::toupper(c);
			REQUIRE(Output.second == "GGTATTTTGTAAGCTGTTATGCCATTTTGGTCTACTAATCAATATTGTAAAAGACATGATGAA");
		}
	}

#pragma endregion GILTests

	//TODO: look at this

	/*
	Testing notes:

	*/
}