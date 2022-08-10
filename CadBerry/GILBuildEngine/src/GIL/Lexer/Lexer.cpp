#include <gilpch.h>
#include "Lexer.h"
#include "LexerTokens.h"
#include "LexerMacros.h"

#include <ctype.h>

namespace GIL
{
	namespace Lexer
	{
		//Function definitions to make our life easier:
		Token* StartsWithLetter(std::string& Text, int& i, size_t& LineNum);
		Token* GetIdent(std::string& Text, int& i, size_t& LineNum);
		Token* GetPrepro(std::string& Text, int& i, size_t& LineNum);
		Token* GetDNALiteral(std::string& Text, int& i, size_t& LineNum);
		Token* GetAminoSequence(std::string& Text, int& i, size_t& LineNum);
		Token* GetComment(std::string& Text, int& i, size_t& LineNum);
		Token* GetMultilineComment(std::string& Text, int& i, size_t& LineNum);
		Token* GetParam(std::string& Text, int& i, size_t& LineNum);
		Token* GetOp(std::string& Text, int& i, size_t& LineNum);

		void GetMapToken(Token*& t, std::unordered_map<std::string, Token**>& Dict, std::string& TokenName, Token* Default);
		void GetMapToken(Token* t, std::unordered_map<std::string, LexerToken>& Dict, LexerToken Default);
		void GetWord(std::string& Text, std::string& Output, int& i);
		void GetString(std::string& Text, std::string& Output, int& i, size_t& LineNum);

		bool ContainsIllegalOperatorChars(std::string& str);



		std::vector<Token*>* Tokenize(std::string& Text)    //The main Tokenize method
		{
			
			std::vector<Token*>* OutputTokens = new std::vector<Token*>();
			size_t LineNum = 0;
			for (int i = 0; i < Text.length(); i++)
			{	
				switch (tolower(Text[i]))
				{
				case '\n':
					++LineNum;
					break;
				case '{':
					OutputTokens->push_back(Token::Begin);
					break;
				case '}':
					OutputTokens->push_back(Token::End);
					break;
				case ',':
					OutputTokens->push_back(Token::Comma);
					break;
				InAlphabet    //Simple macro to check if the char is in the alphabet
				{
					Token* tok = StartsWithLetter(Text, i, LineNum);
					tok->line = LineNum;
					OutputTokens->push_back(tok);
					
					//Now check if the token is an operator definition
					if (tok->TokenType == LexerToken::OPERATOR)
					{
						//Get the operator name
						++i;

						//Advance through whitespace
						while (i < Text.length() && IsWhiteSpace(Text[i]))
						{
							if (Text[i] == '\n')
								++LineNum;
							++i;
						}
						GetWord(Text, tok->Value, i);
					}
					break;
				}
				case '#':
					OutputTokens->push_back(GetPrepro(Text, i, LineNum));
					break;
				case '$':
					++i;
					OutputTokens->push_back(GetParam(Text, i, LineNum));
					break;
				case '@':
					OutputTokens->push_back(GetAminoSequence(Text, i, LineNum));
					break;
				case '/':
					if (Text[i + 1] == '/')
					{
						i += 2;
						OutputTokens->push_back(GetComment(Text, i,	LineNum));
					}
					else if (Text[i + 1] == '*')
					{
						i += 2;
						OutputTokens->push_back(GetMultilineComment(Text, i, LineNum));
					}
					else
					{
						//May be an operator
						Token* t = new Token();
						GetWord(Text, t->Value, i);
						t->TokenType = LexerToken::IDENT;
						OutputTokens->push_back(t);
					}
					break;
				case '.':
					OutputTokens->push_back(GetOp(Text, i, LineNum));
					break;
				case '=':
					if (Text[i + 1] == '>')
					{
						++i;
						OutputTokens->push_back(new Token(LexerToken::FORWARD, ""));
					}
					else
					{
						//May be an operator
						Token* t = new Token();
						GetWord(Text, t->Value, i);
						t->TokenType = LexerToken::IDENT;
						OutputTokens->push_back(t);
					}
					break;
				case '&':
				case '|':
				case '!':
				{
					//Probably an operator
					Token* t = new Token();
					GetWord(Text, t->Value, i);
					t->TokenType = LexerToken::IDENT;
					OutputTokens->push_back(t);
					break;
				}
				case '(':
					OutputTokens->push_back(Token::LParen);
					break;
				case ')':
					OutputTokens->push_back(Token::RParen);
					break;
				case '"':
				{
					Token* NewToken = new Token();
					NewToken->TokenType = LexerToken::STRING;
					++i;
					GetString(Text, NewToken->Value, i, LineNum);
					OutputTokens->push_back(NewToken);
					break;
				}
				case '\'':
					++i;
					OutputTokens->push_back(GetDNALiteral(Text, i, LineNum));
					break;
				case ':':
				{
					//If the last token was a ASSIGNTYPE token, this is a namespace token
					if ((*OutputTokens)[OutputTokens->size() - 1]->TokenType == LexerToken::ASSIGNTYPE)
					{
						(*OutputTokens)[OutputTokens->size() - 1]->TokenType = LexerToken::NAMESPACE;
						break;
					}
					
					//TODO: fix this
					OutputTokens->push_back(new Token(LexerToken::ASSIGNTYPE, ""));
					break;
				}
				case ' ':
				case '\t':
				case '\r':
					break;
				default:
				{
					//Probably an operator
					Token* t = new Token();
					GetWord(Text, t->Value, i);
					
					//Make sure it doesn't contain any illegal characters
					if (ContainsIllegalOperatorChars(t->Value))
					{
						delete t;
						break;
					}
					t->TokenType = LexerToken::IDENT;
					t->line = LineNum;
					OutputTokens->push_back(t);
					break;
				}
				}
			}

			return OutputTokens;
		}

		bool ContainsIllegalOperatorChars(std::string& str)
		{
			for (char c : str)
			{
				//Operators can't contain '"()[]{},.:$@
				if (c == '\'' || c == '"' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == ',' || c == '.' || c == ':' ||  c == '$' || c == '@')
				{
					return true;
				}
			}
			return false;
		}

		Token* StartsWithLetter(std::string& Text, int& i, size_t& LineNum)
		{
			switch (Text[i])
			{
			CouldBeReservedKeyword
				{
					Token* T;
					std::string s;
					GetWord(Text, s, i);

					//Check if word is reserved token
					GetMapToken(T, ReservedKeywords, s, nullptr);
					if (T == nullptr)
					{
						return new Token(LexerToken::IDENT, s);
					}

					//To prevent us from altering the operator token
					if (T->TokenType == LexerToken::OPERATOR)
						return new Token(LexerToken::OPERATOR, "");
					T->line = LineNum;
					return T;
				}
			default:
				return GetIdent(Text, i, LineNum);
			}
		}

		Token* GetIdent(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token(LexerToken::IDENT, "");
			GetWord(Text, T->Value, i);

			T->line = LineNum;
			return T;
		}

		Token* GetPrepro(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token();
			GetWord(Text, T->Value, i);
			GetMapToken(T, PreprocessorDirectives, LexerToken::UNKNOWN);
			if (T->TokenType == LexerToken::UNKNOWN)
			{
				//It's possible that this is actually an operator. 
				if (!ContainsIllegalOperatorChars(T->Value))
				{
					T->TokenType = LexerToken::IDENT;
				}
				else
				{
					CDB_BuildError("Unknown preprocessor directive \"{0}\"", T->Value);
				}
			}
			T->line = LineNum;
			return T;
		}

		Token* GetParam(std::string& Text, int& i, size_t& LineNum)
		{
			std::string s;
			GetWord(Text, s, i);

			Token* T = new Token(LexerToken::PARAM, s);
			T->line = LineNum;
			return T;
		}

		//One change between this version of gil and the c# version of gil is amino acid sequences. In GIL 1, you'd wrap them in AminoSequence {}
		//In this version, you do @Your amino sequence@
		Token* GetAminoSequence(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token(LexerToken::AMINOS, "");
			T->line = LineNum;
			++i;

			int SeqStart = i;
			int SeqLen = 0;
			for (i; i < Text.length() && Text[i] != '@'; ++i)
			{
				if (IsWhiteSpace(Text[i]))
				{
					if (Text[i] == '\n')
					{
						++LineNum;
					}
					continue;
				}
				SeqLen++;
			}
			T->Value.reserve(SeqLen);
			for (SeqStart; SeqStart < i; ++SeqStart)
			{
				if (IsWhiteSpace(Text[SeqStart]))
					continue;
				T->Value += Text[SeqStart];
			}
			return T;
		}

		Token* GetComment(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token();
			T->line = LineNum;
			int CommentStart = i; 
			int NumChars = 0;
			for (i; i < Text.length() && Text[i] != '\n'; ++i)
				++NumChars;
			++LineNum;
			T->TokenType = LexerToken::COMMENT;
			T->Value.reserve(NumChars);
			for (CommentStart; CommentStart < i; ++CommentStart)
				T->Value += Text[CommentStart];
			return T;
		}

		Token* GetMultilineComment(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token();
			T->line = LineNum;
			int CommentStart = i;
			int NumChars = 0;
			for (i; i < Text.length() && (Text[i] != '*' || Text[i + 1] != '/'); ++i)
			{
				if (Text[i] == '\n')
					++LineNum;
				++NumChars;
			}
			T->TokenType = LexerToken::COMMENT;
			T->Value = Text.substr(CommentStart, NumChars);
			return T;
		}

		Token* GetOp(std::string& Text, int& i, size_t& LineNum)
		{
			Token* T = new Token(LexerToken::CALLOP, "");
			T->line = LineNum;
			++i;
			GetWord(Text, T->Value, i);

			return T;
		}




		void GetMapToken(Token*& t, std::unordered_map<std::string, Token**>& Dict, std::string& TokenName, Token* Default)
		{
			t = Default;
			if (Dict.contains(TokenName))
			{
				t = *Dict[TokenName];
			}
		}

		void GetMapToken(Token* t, std::unordered_map<std::string, LexerToken>& Dict, LexerToken Default)
		{
			t->TokenType = Default;

			if (Dict.contains(t->Value))
				t->TokenType = Dict[t->Value];
		}

		void GetWord(std::string& Text, std::string& Output, int& i)
		{
			int WordLen = 0;
			int WordStart = i;
			for (i; i < Text.length() && IsNotWhiteSpace(Text[i]); ++i)
				++WordLen;
			Output.reserve(WordLen);    //Reserve the length of the ident
			for (WordStart; WordStart < i; ++WordStart)
				Output += Text[WordStart];
			--i;
		}

		void GetString(std::string& Text, std::string& Output, int& i, size_t& LineNum)
		{
			int WordLen = 0;
			int WordStart = i;
			for (i; i < Text.length() && Text[i] != '"' && Text[i] != '¬'; ++i)    //We're disallowing ¬ in a string because CadBerry will use that internally to mark entry points
			{
				if (Text[i] == '\n')
					++LineNum;
				++WordLen;
			}
			Output.reserve(WordLen);    //Reserve the length of the string
			for (WordStart; WordStart < i; ++WordStart)
				Output += Text[WordStart];
		}

		Token* GetDNALiteral(std::string& Text, int& i, size_t& LineNum)
		{
			Token* Output = new Token(LexerToken::DNA, "");
			Output->line = LineNum;
			int WordLen = 0;
			int WordStart = i;
			for (i; i < Text.length() && Text[i] != '\''; ++i)
			{
				if (Text[i] == '\n')
					++LineNum;
				++WordLen;
			}
			Output->Value.reserve(WordLen);
			for (WordStart; WordStart < i; ++WordStart)
				Output->Value += Text[WordStart];
			return Output;
		}
	}
}

