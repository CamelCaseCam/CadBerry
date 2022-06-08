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

	extern std::string Empty;

	enum class OperationType : char
	{
		DynamicOperation,
		OperationForward,
	};


	class Operation
	{
	public:
		virtual ~Operation() {}

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) = 0;
		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile) = 0;

		static Operation* LoadOperation(std::ifstream& InputFile);
	};

	class StaticOperation : public Operation
	{
		virtual ~StaticOperation() override;
		std::vector<Parser::Region*> Regions;
		std::string Code;

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}
	};

	class DynamicOperation : public Operation
	{
	public:
		virtual ~DynamicOperation() override;
		std::vector<Lexer::Token*> tokens;
		int NumInnerCode;

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;
	};

	class OperationForward : public Operation
	{
	public:
		OperationForward() { DestinationOperation = nullptr; }
		OperationForward(Operation* destination, std::string& destinationName) : DestinationOperation(destination), DestinationName(destinationName) {}

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;

		Operation* DestinationOperation;
		std::string& DestinationName = Empty;
	};
}