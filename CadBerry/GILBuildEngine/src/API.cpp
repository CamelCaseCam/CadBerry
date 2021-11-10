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

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

std::string DataPath;

using GIL::OutputType;

class GILEXPORT GILBuildEngine : public CDB::BuildEngine
{
public:
	GILBuildEngine() : CDB::BuildEngine({ "gil", "rzyme" }, { "gb", "cgil", "fasta" }) {}
	~GILBuildEngine() override {}

	virtual void PreBuild(std::string& path, std::string& PreBuildPath) override 
	{
		std::ifstream InputFile(path);
		std::stringstream InputText;
		InputText << InputFile.rdbuf();
		std::string FileText = InputText.str();
		auto Tokens = *GIL::Lexer::Tokenize(FileText);
		GIL::Parser::Project* Proj = GIL::Parser::Project::Parse(Tokens);
		CDB_BuildInfo(PreBuildPath);
		Proj->Save(PreBuildPath);
	}

	virtual void Build(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputDir,
		std::string& OutputType) override
	{
		GIL::Parser::Project* Project;
		if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil"))
		{
			CDB_BuildInfo("Precompiled entry point found");
			Project = GIL::Parser::Project::Load(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
		}
		else if (std::filesystem::exists(path + "\\" + EntryPoint + ".gil"))
		{
			std::ifstream t(path + "\\" + EntryPoint + ".gil");
			std::stringstream ss;
			ss << t.rdbuf();

			std::string GILsrc = ss.str();

			auto Tokens = *GIL::Lexer::Tokenize(GILsrc);
			Project = GIL::Parser::Project::Parse(Tokens);
			Project->Save(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
		}
		else
		{
			CDB_BuildError("Could not find entry point \"{0}\" in path \"{1}\"", EntryPoint, path);
			return;
		}

		auto Output = GIL::Compiler::Compile(Project);

		switch (GIL::String2OutputType[OutputType])
		{
		case OutputType::GB:
		{
			std::string GBS = GIL::Compiler::GBSequence::WriteToString(Output, EntryPoint, Project);
			std::ofstream OutputFile(OutputDir + "\\" + EntryPoint + ".gb");
			OutputFile << GBS;
			OutputFile.close();
			break;
		}
		case OutputType::CGIL:
			Project->Save(OutputDir + "\\" + EntryPoint + ".cgil");
			break;
		case OutputType::FASTA:
			break;
		default:
			CDB_BuildFatal("Tried to build to unsupported output type {0}", OutputType);
			break;
		}
	}
};

extern "C"
{
	GILEXPORT CDB::BuildEngine* __stdcall GetBuildEngine()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\Build\\";
		return new GILBuildEngine();
	}
}