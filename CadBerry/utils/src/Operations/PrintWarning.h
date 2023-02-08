#pragma once
#include "GIL.h"

namespace utils
{
	namespace Operations
	{
		class PrintWarning : public GIL::Sequence
		{
		public:
			virtual void Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& Context) override;
			virtual void Save(std::ofstream& OutputFile, GIL::Parser::Project* Proj) override {}
			virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

			static GIL::Sequence* self;
			static GIL::Sequence* GetPtr();
		};
	}
}