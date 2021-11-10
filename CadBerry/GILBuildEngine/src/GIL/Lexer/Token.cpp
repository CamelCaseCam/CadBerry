#include "gilpch.h"
#include "Token.h"
#include "GIL/SaveFunctions.h"

namespace GIL
{
	namespace Lexer
	{
		Token* Token::Newline = new Token(LexerToken::NEWLINE, "");
		Token* Token::EndRegion = new Token(LexerToken::ENDREGION, "");

		Token* Token::Begin = new Token(LexerToken::BEGIN, "");
		Token* Token::End = new Token(LexerToken::END, "");

		Token* Token::InnerCode = new Token(LexerToken::INNERCODE, "");

		Token* Token::UnknownToken = new Token(LexerToken::UNKNOWN, "");

		void Token::Save(std::ofstream& OutputFile)
		{
			OutputFile.write((char*)&this->TokenType, sizeof(LexerToken));
			SaveString(this->Value, OutputFile);
		}

		Token* Token::Load(std::ifstream& InputFile)
		{
			LexerToken TType;
			std::string Value;
			InputFile.read((char*)&TType, sizeof(LexerToken));
			LoadStringFromFile(Value, InputFile);
			switch (TType)
			{
			case LexerToken::NEWLINE:
				return Newline;
			case LexerToken::ENDREGION:
				return EndRegion;
			case LexerToken::BEGIN:
				return Begin;
			case LexerToken::END:
				return End;
			case LexerToken::INNERCODE:
				return InnerCode;
			case LexerToken::UNKNOWN:
				return UnknownToken;
			default:
				return new Token(TType, Value);
			}
		}

		void Token::SafeDelete(Token* t)
		{
			if (t != Newline && t != EndRegion && t != Begin && t != End && t != InnerCode && t != UnknownToken)
			{
				delete t;
			}
		}
	}
}