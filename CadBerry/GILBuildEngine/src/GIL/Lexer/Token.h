#pragma once
#include <gilpch.h>
#include "LexerTokens.h"

#include "Core.h"

namespace GIL
{
	namespace Lexer
	{
		class GILAPI Token
		{
		public:
			Token() {}
			Token(LexerToken tokenType, std::string value) : TokenType(tokenType), Value(value) {}

			void Save(std::ofstream& OutputFile);
			static Token* Load(std::ifstream& InputFile);

			LexerToken TokenType = LexerToken::UNKNOWN;
			std::string Value;
			size_t line = 0;

			/*For any tokens where the value doesn't matter, I'll add a static pointer so that multiple copies are the same object to save
			memory. This may seem like a small improvement, but considering that you could have hundreds of newlines in a single file, it's 
			worth it. Apparently, under one std implementation, an empty string takes up 32 bytes. This means that a Token object takes up 
			at least 36 bytes of space. With potentially 200+ newlines, that's over 7kb wasted!
			*/
			static Token* Newline;
			static Token* EndRegion;

			static Token* Begin;
			static Token* End;

			static Token* DefineSequence;
			static Token* DefOp;
			static Token* InnerCode;

			static Token* Import;
			static Token* Using;

			static Token* From;
			static Token* For;

			static Token* CreateNamespace;

			//Bool stuff
			static Token* IF;
			static Token* Bool;
			static Token* Equals;
			static Token* And;
			static Token* Or;
			static Token* Not;
			static Token* NAND;
			static Token* NOR;

			static Token* LParen;
			static Token* RParen;
			static Token* Comma;

			static Token* True;
			static Token* False;

			static Token* Typedef;
			static Token* Inherits;
			static Token* Operator;
			
			static Token* LBracket;
			static Token* RBracket;

			//I could add a forward (=>) token, but it'll have to be changed anyways into a forwardop or faorwardseq token so I won't


			//This one is absolutely necessary
			static Token* UnknownToken;

			static void SafeDelete(Token* t);    //Checks if the token is one of the global tokens and deletes it if it isn't
		};

		inline std::ostream& operator<<(std::ostream& os, const Token& t)
		{
			return os << LexerToken2Str[t.TokenType] << ", " << t.Value;
		}
	}
}