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
			{"typedef", &Token::Typedef},
			{"inherits", &Token::Inherits},
			{"operator", &Token::Operator},
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

			{"#var", LexerToken::CREATEVAR},
			{"#Var", LexerToken::CREATEVAR},

			{"#inc", LexerToken::INC},
			{"#Inc", LexerToken::INC},

			{"#dec", LexerToken::DEC},
			{"#Dec", LexerToken::DEC},

			{"#if", LexerToken::PREPRO_IF},
			{"#If", LexerToken::PREPRO_IF},

			{"#else", LexerToken::PREPRO_ELSE},
			{"#Else", LexerToken::PREPRO_ELSE},

			{"#endif", LexerToken::PREPRO_ENDIF},
			{"#endIf", LexerToken::PREPRO_ENDIF},
			{"#EndIf", LexerToken::PREPRO_ENDIF},
		});

		std::unordered_map<std::string, Token**> OpRegions = std::unordered_map<std::string, Token**>({    //Stuff like $InnerCode
			{"$InnerCode", &Token::InnerCode}
		});
	}
}