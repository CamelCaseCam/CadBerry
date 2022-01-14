#include <gilpch.h>
#include "LexerMacros.h"

namespace GIL
{
	namespace Lexer
	{
		std::unordered_map<std::string, Token**> ReservedKeywords = std::unordered_map<std::string, Token**>({
			{"sequence", &Token::DefineSequence},
			{"operation", &Token::DefOp},
			{"import", &Token::Import},
			{"if", &Token::IF},
			{"using", &Token::Using},
			{"from", &Token::From},
			{"for", &Token::For},
			{"namespace", &Token::CreateNamespace},
			{"bool", &Token::Bool},
			{"true", &Token::True},
			{"false", &Token::False},
		});

		std::unordered_map<std::string, LexerToken> PreprocessorDirectives = std::unordered_map<std::string, LexerToken>({
			{"#target", LexerToken::SETTARGET},
			{"#Target", LexerToken::SETTARGET},

			{"#SetAttr", LexerToken::SETATTR},
			{"#setAttr", LexerToken::SETATTR},

			{"#Optimize", LexerToken::OPTIMIZE},
			{"#optimize", LexerToken::OPTIMIZE},

			{"#Region", LexerToken::BEGINREGION},
			{"#region", LexerToken::BEGINREGION},

			{"#EndRegion", LexerToken::ENDREGION},
			{"#endRegion", LexerToken::ENDREGION},
		});

		std::unordered_map<std::string, Token**> OpRegions = std::unordered_map<std::string, Token**>({    //Stuff like $InnerCode
			{"$InnerCode", &Token::InnerCode}
		});
	}
}