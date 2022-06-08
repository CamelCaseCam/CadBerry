#include <utilspch.h>
#include "MCS.h"
#include "GIL/Utils/Utils.h"

namespace utils
{
	namespace Operations
	{
		std::pair<std::vector<GIL::Parser::Region>, std::string> MCS::Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj)
		{
			using namespace GIL::Lexer;
			std::string Output;

			for (Token* t : InnerTokens)
			{
				if (t->TokenType == LexerToken::IDENT || t->TokenType == LexerToken::STRING)
				{
					std::string Pattern = GIL::utils::GetRestrictionSite(t->Value);
					Output += GIL::utils::GenWithPattern(Pattern);
				}
			}
			return { {}, Output };
		}

		GIL::Operation* MCS::self = nullptr;
		GIL::Operation* MCS::GetPtr()
		{
			if (self == nullptr)
				self = new MCS();
			return self;
		}
	}
}