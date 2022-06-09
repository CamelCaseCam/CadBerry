#pragma once
#include <gilpch.h>

#include "Core.h"

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

			CREATEVAR,
			INC,
			DEC,
			PREPRO_IF,
			PREPRO_ELSE,
			PREPRO_ENDIF,

			SETTARGET,
			BEGINREGION,
			ENDREGION,

			OPTIMIZE,

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
			COMMA,

			//Type stuff
			ASSIGNTYPE,
			TYPEDEF,
			INHERITS,

			OPERATOR,
		};

		GILAPI extern std::unordered_map<LexerToken, std::string> LexerToken2Str;
	}
}