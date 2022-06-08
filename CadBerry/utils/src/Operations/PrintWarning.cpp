#include <utilspch.h>
#include "PrintWarning.h"

namespace utils
{
	namespace Operations
	{
		std::pair<std::vector<GIL::Parser::Region>, std::string> PrintWarning::Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj)
		{
			if (InnerTokens.size() == 0)
			{
				CDB_BuildWarning("Warning called by utils module");
			}
			else
			{
				CDB_BuildWarning(InnerTokens[0]->Value);
			}
			return std::pair<std::vector<GIL::Parser::Region>, std::string>();
		}

		GIL::Operation* PrintWarning::self = nullptr;
		GIL::Operation* PrintWarning::GetPtr()
		{
			if (self == nullptr)
			{
				self = new PrintWarning();
			}
			return self;
		}
	}
}