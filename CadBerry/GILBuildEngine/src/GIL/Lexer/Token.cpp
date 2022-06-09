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

		Token* Token::DefineSequence = new Token(LexerToken::DEFINESEQUENCE, "");
		Token* Token::DefOp = new Token(LexerToken::DEFOP, "");
		Token* Token::InnerCode = new Token(LexerToken::INNERCODE, "");

		Token* Token::Import = new Token(LexerToken::IMPORT, "");
		Token* Token::Using = new Token(LexerToken::USING, "");

		Token* Token::From = new Token(LexerToken::FROM, "");
		Token* Token::For = new Token(LexerToken::FOR, "");

		Token* Token::CreateNamespace = new Token(LexerToken::CREATENAMESPACE, "");

		//Bool stuff
		Token* Token::IF = new Token(LexerToken::IF, "");
		Token* Token::Bool = new Token(LexerToken::BOOL, "");
		Token* Token::Equals = new Token(LexerToken::EQUALS, "");
		Token* Token::And = new Token(LexerToken::AND, "");
		Token* Token::Or = new Token(LexerToken::OR, "");
		Token* Token::Not = new Token(LexerToken::NOT, "");
		Token* Token::NAND = new Token(LexerToken::NAND, "");
		Token* Token::NOR = new Token(LexerToken::NOR, "");

		Token* Token::LParen = new Token(LexerToken::LPAREN, "");
		Token* Token::RParen = new Token(LexerToken::RPAREN, "");
		Token* Token::Comma = new Token(LexerToken::COMMA, "");

		Token* Token::True = new Token(LexerToken::BOOL_TRUE, "");
		Token* Token::False = new Token(LexerToken::BOOL_FALSE, "");

		Token* Token::Typedef = new Token(LexerToken::TYPEDEF, "");
		Token* Token::Inherits = new Token(LexerToken::INHERITS, "");

		Token* Token::Operator = new Token(LexerToken::OPERATOR, "");

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
			switch (TType)    //If the token is a constant token, return that token
			{
			case LexerToken::NEWLINE:
				return Newline;
			case LexerToken::ENDREGION:
				return EndRegion;

			case LexerToken::BEGIN:
				return Begin;
			case LexerToken::END:
				return End;

			case LexerToken::DEFINESEQUENCE:
				return DefineSequence;
			case LexerToken::DEFOP:
				return DefOp;
			case LexerToken::INNERCODE:
				return InnerCode;

			case LexerToken::IMPORT:
				return Import;
			case LexerToken::USING:
				return Using;

			case LexerToken::FROM:
				return From;
			case LexerToken::FOR:
				return For;

			case LexerToken::CREATENAMESPACE:
				return CreateNamespace;

			case LexerToken::IF:
				return IF;
			case LexerToken::BOOL:
				return Bool;
			case LexerToken::EQUALS:
				return Equals;
			case LexerToken::AND:
				return And;
			case LexerToken::OR:
				return Or;
			case LexerToken::NOT:
				return Not;
			case LexerToken::NAND:
				return NAND;
			case LexerToken::NOR:
				return NOR;

			case LexerToken::BOOL_TRUE:
				return True;
			case LexerToken::BOOL_FALSE:
				return False;

			case LexerToken::LPAREN:
				return LParen;
			case LexerToken::RPAREN:
				return RParen;

			case LexerToken::OPERATOR:
				return Operator;

			case LexerToken::UNKNOWN:
				return UnknownToken;
			default:
				return new Token(TType, Value);
			}
		}

		void Token::SafeDelete(Token* t)
		{
			switch (t->TokenType)
			{
			case LexerToken::NEWLINE:
			case LexerToken::ENDREGION:

			case LexerToken::BEGIN:
			case LexerToken::END:

			case LexerToken::DEFINESEQUENCE:
			case LexerToken::DEFOP:
			case LexerToken::INNERCODE:

			case LexerToken::IMPORT:
			case LexerToken::USING:

			case LexerToken::FROM:
			case LexerToken::FOR:

			case LexerToken::CREATENAMESPACE:

			case LexerToken::IF:
			case LexerToken::BOOL:
			case LexerToken::EQUALS:
			case LexerToken::AND:
			case LexerToken::OR:
			case LexerToken::NOT:
			case LexerToken::NAND:
			case LexerToken::NOR:

			case LexerToken::BOOL_TRUE:
			case LexerToken::BOOL_FALSE:

			case LexerToken::LPAREN:
			case LexerToken::RPAREN:

			case LexerToken::TYPEDEF:
			case LexerToken::INHERITS:

			case LexerToken::OPERATOR:

			case LexerToken::UNKNOWN:
				break;
			default:
				delete t;
			}
		}
	}
}