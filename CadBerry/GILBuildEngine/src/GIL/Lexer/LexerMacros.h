#pragma once
#include "LexerTokens.h"
#include "Token.h"
#include <gilpch.h>


#define InAlphabet case 'a':\
case 'b':\
case 'c':\
case 'd':\
case 'e':\
case 'f':\
case 'g':\
case 'h':\
case 'i':\
case 'j':\
case 'k':\
case 'l':\
case 'm':\
case 'n':\
case 'o':\
case 'p':\
case 'q':\
case 'r':\
case 's':\
case 't':\
case 'u':\
case 'v':\
case 'w':\
case 'x':\
case 'y':\
case 'z':

/*
GIL Reserved Keywords:
sequence
operation
import
using
from 
for
*/
#define CouldBeReservedKeyword case 's':\
case 'o':\
case 'i':\
case 'u':\
case 'f':\
case 'n':\
case 'b':\
case 't':

#define IsWhiteSpace(x) x == ' ' || x == '\n' || x == '\r' || x == '\t'
#define IsNotWhiteSpace(x) x != ' ' && x != '\n' && x != '\r' && x != '\t' && x != ':' && x != ')' && x != '('

namespace GIL
{
	namespace Lexer
	{
		extern std::unordered_map<std::string, Token**> ReservedKeywords;


		/*
		GIL Preprocessor directives:
		#SetTarget
		#setTarget

		#Region
		#region

		#EndRegion
		#endRegion
		*/
		extern std::unordered_map<std::string, LexerToken> PreprocessorDirectives;

		extern std::unordered_map<std::string, Token**> OpRegions;
	}
}