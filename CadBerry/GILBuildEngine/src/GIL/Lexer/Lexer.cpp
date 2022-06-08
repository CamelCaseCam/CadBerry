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
		Token* StartsWithLetter(std::string& Text, int& i);
		Token* GetIdent(std::string& Text, int& i);
		Token* GetPrepro(std::string& Text, int& i);
		Token* GetDNALiteral(std::string& Text, int& i);
		Token* GetAminoSequence(std::string& Text, int& i);
		Token* GetComment(std::string& Text, int& i);
		Token* GetMultilineComment(std::string& Text, int& i);
		Token* GetParam(std::string& Text, int& i);
		Token* GetOp(std::string& Text, int& i);

		void GetMapToken(Token*& t, std::unordered_map<std::string, Token**>& Dict, std::string& TokenName, Token* Default);
		void GetMapToken(Token* t, std::unordered_map<std::string, LexerToken>& Dict, LexerToken Default);
		void GetWord(std::string& Text, std::string& Output, int& i);
		void GetString(std::string& Text, std::string& Output, int& i);



		std::vector<Token*>* Tokenize(std::string& Text)    //The main Tokenize method
		{
			std::vector<Token*>* OutputTokens = new std::vector<Token*>();
			for (int i = 0; i < Text.length(); i++)
			{
				switch (tolower(Text[i]))
				{
				case '\n':
					OutputTokens->push_back(Token::Newline);
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
					OutputTokens->push_back(StartsWithLetter(Text, i));
					break;
				case '#':
					OutputTokens->push_back(GetPrepro(Text, i));
					break;
				case '$':
					++i;
					OutputTokens->push_back(GetParam(Text, i));
					break;
				case '@':
					OutputTokens->push_back(GetAminoSequence(Text, i));
					break;
				case '/':
					if (Text[i + 1] == '/')
					{
						i += 2;
						OutputTokens->push_back(GetComment(Text, i));
					}
					else if (Text[i + 1] == '*')
					{
						i += 2;
						OutputTokens->push_back(GetMultilineComment(Text, i));
					}
					break;
				case '.':
					OutputTokens->push_back(GetOp(Text, i));
					break;
				case '=':
					if (Text[i + 1] == '>')
					{
						++i;
						OutputTokens->push_back(new Token(LexerToken::FORWARD, ""));
					}
					else
					{
						OutputTokens->push_back(Token::Equals);
					}
					break;
				case '&':
					OutputTokens->push_back(Token::And);
					break;
				case '|':
					OutputTokens->push_back(Token::Or);
					break;
				case '!':
					if (Text.length() > i + 1)
					{
						if (Text[i + 1] == '&')    //!&
						{
							OutputTokens->push_back(Token::NAND);
						}
						else if (Text[i + 1] == '|')    //!|
						{
							OutputTokens->push_back(Token::NOR);
						}
						else
						{
							OutputTokens->push_back(Token::Not);
						}
					}
					else
					{
						OutputTokens->push_back(Token::Not);
					}
					break;
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
					GetString(Text, NewToken->Value, i);
					OutputTokens->push_back(NewToken);
					break;
				}
				case '\'':
					++i;
					OutputTokens->push_back(GetDNALiteral(Text, i));
					break;
				case ':':
				{
					//If the last token was a namespace token, ignore this colon
					if ((*OutputTokens)[OutputTokens->size() - 1]->TokenType == LexerToken::NAMESPACE)
					{
						break;
					}
					
					//Check if the last token was an ident
					if (OutputTokens->operator[](OutputTokens->size() - 1)->TokenType != LexerToken::IDENT)
					{
						if ((*OutputTokens)[OutputTokens->size() - 1]->TokenType == LexerToken::RPAREN)
						{
							//TODO: fix this
							OutputTokens->push_back(new Token(LexerToken::ASSIGNTYPE, ""));
							break;
						}
						CDB_BuildError("Expected namespace name");
					}

					//if it's a type assignment
					if ((*OutputTokens)[OutputTokens->size() - 2]->TokenType == LexerToken::DEFINESEQUENCE
						|| (*OutputTokens)[OutputTokens->size() - 2]->TokenType == LexerToken::DEFOP)
					{
						//TODO: fix this
						OutputTokens->push_back(new Token(LexerToken::ASSIGNTYPE, ""));
						break;
					}
					OutputTokens->operator[](OutputTokens->size() - 1)->TokenType = LexerToken::NAMESPACE;
					break;
				}
				default:
					break;
				}
			}

			return OutputTokens;
		}

		Token* StartsWithLetter(std::string& Text, int& i)
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
					return T;
				}
			default:
				return GetIdent(Text, i);
			}
		}

		Token* GetIdent(std::string& Text, int& i)
		{
			Token* T = new Token(LexerToken::IDENT, "");
			GetWord(Text, T->Value, i);

			return T;
		}

		Token* GetPrepro(std::string& Text, int& i)
		{
			Token* T = new Token();
			GetWord(Text, T->Value, i);
			GetMapToken(T, PreprocessorDirectives, LexerToken::UNKNOWN);
			if (T->TokenType == LexerToken::UNKNOWN)
			{
				CDB_BuildError("Unknown preprocessor command {0}", *T);
				return T;
			}
			return T;
		}

		Token* GetParam(std::string& Text, int& i)
		{
			std::string s;
			GetWord(Text, s, i);

			return new Token(LexerToken::PARAM, s);
		}

		//One change between this version of gil and the c# version of gil is amino acid sequences. In GIL 1, you'd wrap them in AminoSequence {}
		//In this version, you do @Your amino sequence@
		Token* GetAminoSequence(std::string& Text, int& i)
		{
			Token* T = new Token(LexerToken::AMINOS, "");
			++i;

			int SeqStart = i;
			int SeqLen = 0;
			for (i; i < Text.length() && Text[i] != '@'; ++i)
			{
				if (IsWhiteSpace(Text[i]))
					continue;
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

		Token* GetComment(std::string& Text, int& i)
		{
			Token* T = new Token();
			int CommentStart = i; 
			int NumChars = 0;
			for (i; i < Text.length() && Text[i] != '\n'; ++i)
				++NumChars;
			T->TokenType = LexerToken::COMMENT;
			T->Value.reserve(NumChars);
			for (CommentStart; CommentStart < i; ++CommentStart)
				T->Value += Text[CommentStart];
			return T;
		}

		Token* GetMultilineComment(std::string& Text, int& i)
		{
			Token* T = new Token();
			int CommentStart = i;
			int NumChars = 0;
			for (i; i < Text.length() && (Text[i] != '*' || Text[i + 1] != '/'); ++i)
				++NumChars;
			T->TokenType = LexerToken::COMMENT;
			T->Value = Text.substr(CommentStart, NumChars);
			return T;
		}

		Token* GetOp(std::string& Text, int& i)
		{
			Token* T = new Token(LexerToken::CALLOP, "");
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

		void GetString(std::string& Text, std::string& Output, int& i)
		{
			int WordLen = 0;
			int WordStart = i;
			for (i; i < Text.length() && Text[i] != '"' && Text[i] != '¬'; ++i)    //We're disallowing ¬ in a string because CadBerry will use that internally to mark entry points
				++WordLen;
			Output.reserve(WordLen);    //Reserve the length of the string
			for (WordStart; WordStart < i; ++WordStart)
				Output += Text[WordStart];
		}

		Token* GetDNALiteral(std::string& Text, int& i)
		{
			Token* Output = new Token(LexerToken::DNA, "");
			int WordLen = 0;
			int WordStart = i;
			for (i; i < Text.length() && Text[i] != '\''; ++i)
				++WordLen;
			Output->Value.reserve(WordLen);
			for (WordStart; WordStart < i; ++WordStart)
				Output->Value += Text[WordStart];
			return Output;
		}
	}
}

