#include <gilpch.h>
#include "LexerMacros.h"

namespace GIL
{
	namespace Lexer
	{
		std::unordered_map<std::string, LexerToken> ReservedKeywords = std::unordered_map<std::string, LexerToken>({
			{"sequence", LexerToken::DEFINESEQUENCE},
			{"operation", LexerToken::DEFOP},
			{"import", LexerToken::IMPORT},
			{"using", LexerToken::USING},
			{"from", LexerToken::FROM},
			{"for", LexerToken::FOR}
		});

		std::unordered_map<std::string, LexerToken> PreprocessorDirectives = std::unordered_map<std::string, LexerToken>({
			{"#target", LexerToken::SETTARGET},
			{"#Target", LexerToken::SETTARGET},

			{"#Region", LexerToken::BEGINREGION},
			{"#region", LexerToken::BEGINREGION},

			{"#EndRegion", LexerToken::ENDREGION},
			{"#endRegion", LexerToken::ENDREGION},
		});

		std::unordered_map<std::string, LexerToken> OpRegions = std::unordered_map<std::string, LexerToken>({    //Stuff like $InnerCode
			{"$InnerCode", LexerToken::INNERCODE}
		});
	}
}