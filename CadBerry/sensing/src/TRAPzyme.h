#pragma once
#include "GIL.h"

namespace sensing
{
	class SetTRAPzymeTarget : public GIL::Sequence
	{
	public:
		virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& context) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

		static GIL::Sequence* self;
		static GIL::Sequence* GetPtr();
	};

	class GenTZTarget : public GIL::Sequence
	{
	public:
		virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& context) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

		static GIL::Sequence* self;
		static GIL::Sequence* GetPtr();
	};

	class GenTZFromAttenuator : public GIL::Sequence
	{
	public:
		virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& context) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

		static GIL::Sequence* self;
		static GIL::Sequence* GetPtr();
	};

	class GenTZV2FromAttenuator : public GIL::Sequence
	{
	public:
		virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& context) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

		static GIL::Sequence* self;
		static GIL::Sequence* GetPtr();
	};

	class CreateAttenuator : public GIL::Sequence
	{
	public:
		virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& context) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

		static GIL::Sequence* self;
		static GIL::Sequence* GetPtr();
	};
}