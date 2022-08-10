#pragma once
#include "GIL.h"

namespace utils
{
	namespace Operations
	{
		class MCS : public GIL::Sequence
		{
		public:
			virtual void Get(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& Context) override;
			virtual void Save(std::ofstream& OutputFile) override {}
			virtual void Load(std::ifstream& InputFile, GIL::Parser::Project* Proj) override {}

			static GIL::Sequence* self;
			static GIL::Sequence* GetPtr();
		};
	}
}