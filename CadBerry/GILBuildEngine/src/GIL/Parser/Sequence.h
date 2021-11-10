#pragma once
#include <cdbpch.h>
#include "Region.h"
#include "GIL/Lexer/Token.h"


namespace GIL
{
	namespace Parser
	{
		class Project;
	}



	class Sequence
	{
	public:
		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) = 0;
		virtual std::string* GetCode(Parser::Project* Proj) = 0;

		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile) = 0;
	};

	class StaticSequence : public Sequence
	{
	public:
		StaticSequence() {}
		StaticSequence(std::vector<GIL::Lexer::Token*> tokens) : Tokens(tokens) {}
		~StaticSequence();
		bool IsCompiled = false;
		std::vector<GIL::Lexer::Token*> Tokens;
		std::vector<Parser::Region> Regions;
		std::string Code;

		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) override;
		virtual std::string* GetCode(Parser::Project* Proj) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;
	};
}