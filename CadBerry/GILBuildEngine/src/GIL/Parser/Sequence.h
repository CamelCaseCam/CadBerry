#pragma once
#include <cdbpch.h>
#include "Region.h"
#include "GIL/Lexer/Token.h"
#include "Operation.h"


namespace GIL
{
	namespace Parser
	{
		class Project;
	}



	class Sequence    //Base sequence class that is inherited by different sequence types
	{
	public:
		virtual ~Sequence() {}
		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) = 0;
		virtual std::string* GetCode(Parser::Project* Proj) = 0;

		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile) = 0;

		static Sequence* LoadSequence(std::ifstream& InputFile);
	};

	class StaticSequence : public Sequence    //So far we only have static sequences
	{
	public:
		StaticSequence() {}
		StaticSequence(std::vector<GIL::Lexer::Token*> tokens) : Tokens(tokens) {}
		~StaticSequence() override;
		bool IsCompiled = false;
		std::vector<GIL::Lexer::Token*> Tokens;
		std::vector<Parser::Region> Regions;
		std::string Code;

		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) override;
		virtual std::string* GetCode(Parser::Project* Proj) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;
	};

	class SequenceForward : public Sequence
	{
	public:
		SequenceForward() { DestinationSequence = nullptr; }
		SequenceForward(Sequence* destination, std::string& destinationName) : DestinationSequence(destination), DestinationName(destinationName) {}

		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) override;
		virtual std::string* GetCode(Parser::Project* Proj) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile) override;

		Sequence* DestinationSequence;
		std::string& DestinationName = Empty;
	};
}