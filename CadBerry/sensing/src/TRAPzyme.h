#pragma once
#include "GIL.h"

namespace sensing
{
	class SetTRAPzymeTarget : public GIL::Operation
	{
	public:
		virtual std::pair<std::vector<GIL::Parser::Region>, std::string> Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}

		static GIL::Operation* self;
		static GIL::Operation* GetPtr();
	};

	class GenTZFromAttenuator : public GIL::Operation
	{
	public:
		virtual std::pair<std::vector<GIL::Parser::Region>, std::string> Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}

		static GIL::Operation* self;
		static GIL::Operation* GetPtr();
	};

	class CreateAttenuator : public GIL::Operation
	{
	public:
		virtual std::pair<std::vector<GIL::Parser::Region>, std::string> Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}

		static GIL::Operation* self;
		static GIL::Operation* GetPtr();
	};
}