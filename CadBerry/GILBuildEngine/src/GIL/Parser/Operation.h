#pragma once
#include <gilpch.h>
#include "GIL/Lexer/Token.h"
#include "GIL/Lexer/LexerMacros.h"
#include "GIL/Lexer/LexerTokens.h"
#include "Region.h"

namespace GIL
{
	namespace Parser
	{
		class Project;
	}


	class Operation
	{
	public:
		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) = 0;
		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile) = 0;
	};

	class StaticOperation : public Operation
	{
		std::vector<Parser::Region*> Regions;
		std::string Code;

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}
	};

	class DynamicOperation : public Operation
	{
	public:
		std::vector<Lexer::Token*> tokens;
		int NumInnerCode;

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;
	};
}