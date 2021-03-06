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
		Token* GetAminoSequence(std::string& Text, int& i);
		Token* GetComment(std::string& Text, int& i);
		Token* GetMultilineComment(std::string& Text, int& i);
		Token* GetIfInnerCode(std::string& Text, int& i);
		Token* GetOp(std::string& Text, int& i);

		void GetMapToken(Token* t, std::unordered_map<std::string, LexerToken>& Dict, LexerToken Default);
		void GetWord(std::string& Text, std::string& Output, int& i);



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
				InAlphabet    //Simple macro to check if the char is in the alphabet
					OutputTokens->push_back(StartsWithLetter(Text, i));
					break;
				case '#':
					OutputTokens->push_back(GetPrepro(Text, i));
					break;
				case '$':
					OutputTokens->push_back(GetIfInnerCode(Text, i));
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
					break;
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
					Token* T = new Token();
					GetWord(Text, T->Value, i);

					//Check if word is reserved token
					GetMapToken(T, ReservedKeywords, LexerToken::IDENT);
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

		Token* GetIfInnerCode(std::string& Text, int& i)
		{
			Token* T = new Token();
			GetWord(Text, T->Value, i);
			GetMapToken(T, OpRegions, LexerToken::UNKNOWN);
			if (T->TokenType == LexerToken::UNKNOWN)
			{
				CDB_BuildError("Error {0}", *T);
				return T;
			}
			return T;
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
			T->Value.reserve(NumChars);
			for (CommentStart; CommentStart < i; ++CommentStart)
				T->Value += Text[CommentStart];
			return T;
		}

		Token* GetOp(std::string& Text, int& i)
		{
			Token* T = new Token(LexerToken::CALLOP, "");
			++i;
			GetWord(Text, T->Value, i);

			return T;
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
		}
	}
}
