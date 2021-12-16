#pragma once
#include <gilpch.h>

namespace GIL
{
	namespace Lexer
	{
		enum class LexerToken
		{
			IMPORT,
			STRING,
			USING,
			IDENT,
			NEWLINE,
			COMMENT,
			SETATTR,
			PARAM,

			SETTARGET,
			BEGINREGION,
			ENDREGION,

			AMINOS,
			DNA,

			BEGIN,
			END,

			FROM,
			FOR,

			DEFINESEQUENCE,
			FORWARD,
			FORWARDSEQ,
			DEFOP,
			STATOP,
			FORWARDOP,
			INNERCODE,
			CALLOP,

			DEFPROMOTER,
			CALLPROMOTER,
			DEFGENE,

			UNKNOWN,

			NAMESPACE,
			CREATENAMESPACE,

			//Bool stuff
			IF,
			BOOL,
			EQUALS,
			AND,
			OR,
			NOT,
			NAND,
			NOR,

			BOOL_TRUE,
			BOOL_FALSE,

			RPAREN,
			LPAREN,
		};

		extern std::unordered_map<LexerToken, std::string> LexerToken2Str;
	}
}