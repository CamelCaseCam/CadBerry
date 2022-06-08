#pragma once
#include "GIL.h"

namespace utils
{
	namespace Operations
	{
		class PrintWarning : public GIL::Operation
		{
		public:
			virtual std::pair<std::vector<GIL::Parser::Region>, std::string> Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj) override;
			virtual void Save(std::ofstream& OutputFile) override {}
			virtual void Load(std::ifstream& InputFile) override {}

			static GIL::Operation* self;
			static GIL::Operation* GetPtr();
		};
	}
}