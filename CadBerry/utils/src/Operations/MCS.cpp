#include <utilspch.h>
#include "MCS.h"
#include "GIL/Utils/Utils.h"

namespace utils
{
	namespace Operations
	{
		void MCS::Get_impl(GIL::Parser::Project* Proj, std::map<std::string, GIL::Param>& Params, GIL::Compiler::CompilerContext& Context)
		{
			using namespace GIL::Lexer;
			std::string Output;

			CDB_BuildWarning("MCS operation in utils module is not functional");
			/*for (Token* t : InnerTokens)
			{
				if (t->TokenType == LexerToken::IDENT || t->TokenType == LexerToken::STRING)
				{
					std::string Pattern = GIL::utils::GetRestrictionSite(t->Value);
					Output += GIL::utils::GenWithPattern(Pattern);
				}
			}*/
			Context.OutputString->append(Output);
		}

		GIL::Sequence* MCS::self = nullptr;
		GIL::Sequence* MCS::GetPtr()
		{
			if (self == nullptr)
				self = new MCS();
			return self;
		}
	}
}