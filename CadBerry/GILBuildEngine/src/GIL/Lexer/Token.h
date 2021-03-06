#pragma once
#include <gilpch.h>
#include "LexerTokens.h"

namespace GIL
{
	namespace Lexer
	{
		class Token
		{
		public:
			Token() {}
			Token(LexerToken tokenType, std::string value) : TokenType(tokenType), Value(value) {}

			void Save(std::ofstream& OutputFile);
			static Token* Load(std::ifstream& InputFile);

			LexerToken TokenType;
			std::string Value;

			/*For any tokens where the value doesn't matter, I'll add a static pointer so that multiple copies are the same object to save
			memory. This may seem like a small improvement, but considering that you could have hundreds of newlines in a single file, it's 
			worth it. Apparently, under one std implementation, an empty string takes up 32 bytes. This means that a Token object takes up 
			at least 36 bytes of space. With potentially 200+ newlines, that's over 7kb wasted!
			*/
			static Token* Newline;
			static Token* EndRegion;

			static Token* Begin;
			static Token* End;

			static Token* InnerCode;

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