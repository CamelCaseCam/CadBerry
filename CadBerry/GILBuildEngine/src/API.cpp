#include <gilpch.h>
#define FMT_HEADER_ONLY
#include "CadBerry.h"
#include "GIL/Lexer/Lexer.h"
#include "GIL/Lexer/Token.h"
#include "GIL/Parser/Project.h"
#include "GIL/Examples.h"
#include "GIL/Compiler/Compiler.h"

#include "GIL/OutputTypes.h"
#include "GIL/Compiler/GBSequence.h"

#include "GIL/Errors.h"

#include "GIL/Bools/BoolContext.h"
#include "GIL/Utils/Utils.h"

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

std::string DataPath;

using GIL::OutputType;

class GILAPI GILBuildEngine : public CDB::BuildEngine
{
public:
	GILBuildEngine() : CDB::BuildEngine({ "gil", "rzyme" }, { "gb", "cgil", "fasta" }) {}
	~GILBuildEngine() override {}

	virtual void PreBuild(std::string& path, std::string& PreBuildPath) override 
	{
		//Read all text from the file
		std::ifstream InputFile(path);
		std::stringstream InputText;
		InputText << InputFile.rdbuf();
		std::string FileText = InputText.str();

		auto Tokens = *GIL::Lexer::Tokenize(FileText);
		CheckFatal;
		GIL::Parser::Project* Proj = GIL::Parser::Project::Parse(Tokens);
		CheckFatal;

		GIL::BoolContext::Print();
		
		Proj->Save(PreBuildPath);

		delete Proj;
	}

	virtual void Build(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputDir,
		std::string& OutputType) override
	{
		GIL::Parser::Project* Project;
		if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil"))    //Check if precompiled version exists
		{
			CDB_BuildInfo("Precompiled entry point found");
			Project = GIL::Parser::Project::Load(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
		}
		else if (std::filesystem::exists(path + "\\" + EntryPoint + ".gil"))    //Make sure the source code for the entry point exists
		{
			std::ifstream t(path + "\\" + EntryPoint + ".gil");
			std::stringstream ss;
			ss << t.rdbuf();

			std::string GILsrc = ss.str();	//Read the file code

			auto Tokens = *GIL::Lexer::Tokenize(GILsrc);
			CheckFatal;
			Project = GIL::Parser::Project::Parse(Tokens);
			CheckFatal;
			Project->Save(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
		}
		else
		{
			CDB_BuildError("Could not find entry point \"{0}\" in path \"{1}\"", EntryPoint, path);
			return;
		}

		auto Output = GIL::Compiler::Compile(Project);	//Compile code into intermediate output
		CheckFatal;

		switch (GIL::String2OutputType[OutputType])
		{
		case OutputType::GB:
		{
			std::string GBS = GIL::Compiler::GBSequence::WriteToString(Output, EntryPoint, Project);	//Convert intermediate output to GenBenk text
			
			//Write GenBank code to file
			std::ofstream OutputFile(OutputDir + "\\" + EntryPoint + ".gb");
			OutputFile << GBS;
			OutputFile.close();
			break;
		}
		case OutputType::CGIL:
			Project->Save(OutputDir + "\\" + EntryPoint + ".cgil");    //Just save the library
			break;
		case OutputType::FASTA:    //To be implemented
			break;
		default:
			CDB_BuildFatal("Tried to build to unsupported output type {0}", OutputType);
			break;
		}

		//Cleanup
		delete Project;
	}
};

//Expose the module to CadBerry
extern "C"
{
	__declspec(dllexport) CDB::BuildEngine* __stdcall GetBuildEngine()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\Build\\";
		return new GILBuildEngine();
	}
}