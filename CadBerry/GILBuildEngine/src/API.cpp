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

#include "GIL/Utils/Utils.h"

#include "GIL/GILException.h"

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

std::string DataPath;

//TODO: Find some way to prevent compilation if precompilation is happening, but still allow multiple precompilations at the same time (maybe using mutex)
//TODO: Replace CheckFatal macro in GILBuildEngine with exceptions

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
		GIL::Parser::Project* Proj = GIL::Parser::Project::Parse(Tokens);
		
		if (!std::filesystem::exists(std::filesystem::path(PreBuildPath).parent_path()))
		{
			std::filesystem::create_directories(std::filesystem::path(PreBuildPath).parent_path());
		}
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

			try
			{
				auto Tokens = *GIL::Lexer::Tokenize(GILsrc);
				Project = GIL::Parser::Project::Parse(Tokens);
				Project->Save(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
			}
			catch (GIL::GILException& exception)
			{
				CDB_BuildError("GIL encountered a fatal error, terminating compilation");
				return;
			}
		}
		else
		{
			CDB_BuildError("Could not find entry point \"{0}\" in path \"{1}\"", EntryPoint, path);
			return;
		}

		try
		{
			auto Output = GIL::Compiler::Compile(Project);	//Compile code into intermediate output

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
		}
		catch (GIL::GILException& exception)
		{
			CDB_BuildError("GIL encountered a fatal error, terminating compilation");
			return;
		}

		//Cleanup
		delete Project;
	}

	virtual void* BuildConsole(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputType) override
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
			Project = GIL::Parser::Project::Parse(Tokens);
			Project->Save(CDB::Application::Get().PreBuildDir + "\\" + EntryPoint + ".cgil");
		}
		else
		{
			CDB_BuildError("Could not find entry point \"{0}\" in path \"{1}\"", EntryPoint, path);
			return nullptr;
		}

		auto Output = new std::pair<std::vector<GIL::Parser::Region>, std::string>();
		*Output = GIL::Compiler::Compile(Project);	//Compile code into intermediate output and write it to pointer

		delete Project;
		return Output;
	}


	virtual void* BuildConsole(std::string src, std::string OutputType) override
	{
		GIL::Parser::Project* Project;

		auto Tokens = *GIL::Lexer::Tokenize(src);
		Project = GIL::Parser::Project::Parse(Tokens);

		auto Output = new std::pair<std::vector<GIL::Parser::Region>, std::string>();
		*Output = GIL::Compiler::Compile(Project);	//Compile code into intermediate output and write it to pointer

		delete Project;
		return Output;
	}




	std::string CreateEntryPoint(std::string& TargetOrganism, std::vector<std::string>& EntrySequences) override
	{
		std::string Output = "#Target " + TargetOrganism + "\n";

		//Get all entry sequences
		for (std::string& sequence : EntrySequences)
		{
			auto SequenceLocation = GIL::utils::FindSequenceInIntermediates(sequence);
			if (SequenceLocation.first != "")
			{
				Output += "import \"" + SequenceLocation.first + "\"\n";
				Output += SequenceLocation.second + "::" + sequence;
			}
			else
			{
				CDB_BuildError("Couldn't find sequence \"{0}\" in precompiled files", sequence);
			}
		}
		CDB_BuildInfo(Output);
		return Output;
	}
};

//Expose the module to CadBerry
extern "C"
{
#ifdef CDB_PLATFORM_WINDOWS
	__declspec(dllexport) CDB::BuildEngine* __stdcall GetBuildEngine()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\Build\\";
		return new GILBuildEngine();
	}
#else
	CDB::BuildEngine* GetBuildEngine()
	{
		DataPath = CDB::Application::Get().PathToEXE + "\\Build\\";
		return new GILBuildEngine();
	}
#endif
}