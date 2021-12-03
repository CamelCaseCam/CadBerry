#include <gilpch.h>
#include "LexerTokens.h"

namespace GIL
{
	namespace Lexer
	{
		std::unordered_map<LexerToken, std::string> LexerToken2Str = std::unordered_map<LexerToken, std::string>({
			{LexerToken::IMPORT, "import"},
			{LexerToken::STRING, "string"},
			{LexerToken::USING, "using"},
			{LexerToken::IDENT, "ident"},
			{LexerToken::NEWLINE, "newline"},
			{LexerToken::COMMENT, "comment"},
			{LexerToken::SETATTR, "set attribute"},
			{LexerToken::PARAM, "param"},
			{LexerToken::SETTARGET, "set target"},
			{LexerToken::BEGINREGION, "begin region"},
			{LexerToken::ENDREGION, "end region"},

			{LexerToken::AMINOS, "aminos"},
			{LexerToken::DNA, "dna"},

			{LexerToken::BEGIN, "begin"},
			{LexerToken::END, "end"},

			{LexerToken::FROM, "from"},
			{LexerToken::FOR, "for"},

			{LexerToken::DEFINESEQUENCE, "define sequence"},
			{LexerToken::FORWARD, "forward"},
			{LexerToken::FORWARDSEQ, "forward sequence"},
			{LexerToken::DEFOP, "define operation"},
			{LexerToken::STATOP, "define static operation"},
			{LexerToken::FORWARDOP, "forward operation"},
			{LexerToken::INNERCODE, "innercode"},
			{LexerToken::CALLOP, "call operation"},

			{LexerToken::DEFPROMOTER, "define promoter"},
			{LexerToken::CALLPROMOTER, "call promoter"},
			{LexerToken::DEFGENE, "define gene"},


			{LexerToken::UNKNOWN, "unknown token"},

			{LexerToken::NAMESPACE, "namespace"},
			{LexerToken::CREATENAMESPACE, "create namespace"},

			{LexerToken::IF, "if"},
			{LexerToken::BOOL, "bool"},
			{LexerToken::EQUALS, "equals"},
			{LexerToken::AND, "and"},
			{LexerToken::OR, "or"},
			{LexerToken::NOT, "not"},
			{LexerToken::NAND, "nand"},
			{LexerToken::NOR, "nor"},

			{LexerToken::LPAREN, "left parentheses"},
			{LexerToken::RPAREN, "right parentheses"},
		});
	}
}