#pragma once

//Handmade generational parser made to create the algorithm

//Header Prologue
#include <gilpch.h>
#include "AST_Node.h"
#include "GIL/Lexer/Token.h"
//End prologue

//Imports
#include <memory>
#include <vector>
#include <unordered_map>

//Token type and value typedefs
namespace GenParser
{
	typedef GIL::Lexer::LexerToken TokenType;
	typedef std::string TokenValue;

	//Define External_Token_Implementation to a class/struct with a type and value member
#ifndef External_Token_Implementation
	struct Token
	{
		TokenType Type;
		TokenValue Value;
	};
#else
	typedef External_Token_Implementation Token;
#endif

	enum class ParserElementType
	{
		//Tokens
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
		
		//Nodes
		LexerToken,
		SET_TARGET,
		SET_ATTR,
		SET_VAR,
		BEGIN_REGION,
		END_REGION,
		INC_VAR,
		DEC_VAR,
		P_PARAM,
		P_IMPORT,
		P_USING,
		P_OPERATOR,
		P_FORWARD,
		CALL_PARAM,
		NAMESPACE_ACCESSION,
		DNA_LITERAL,
		AMINO_ACID_LITERAL,
		P_TYPEDEF,
		P_INHERITS,
		COMMENT,
		PARAM_DEFINITION_GROUP,
		CALL_PARAMS,
		CALL_SEQUENCE,
		P_CALLOP,
		STATEMENT,
		P_PREPRO_IF,
		P_PREPRO_ELSE,
		P_FROM,
		P_FOR,
		DEFINE_SEQUENCE,
		DEFINE_OPERATION,
		CREATE_NAMESPACE,
	};
	
	std::unordered_map<TokenType, ParserElementType> tokenToParserElementType = {
		{TokenType::IMPORT, ParserElementType::IMPORT},
		{TokenType::STRING, ParserElementType::STRING},
		{TokenType::USING, ParserElementType::USING},
		{TokenType::IDENT, ParserElementType::IDENT},
		{TokenType::NEWLINE, ParserElementType::NEWLINE},
		{TokenType::COMMENT, ParserElementType::COMMENT},
		{TokenType::SETATTR, ParserElementType::SETATTR},
		{TokenType::PARAM, ParserElementType::PARAM},

		{TokenType::CREATEVAR, ParserElementType::CREATEVAR},
		{TokenType::INC, ParserElementType::INC},
		{TokenType::DEC, ParserElementType::DEC},
		{TokenType::PREPRO_IF, ParserElementType::PREPRO_IF},
		{TokenType::PREPRO_ELSE, ParserElementType::PREPRO_ELSE},
		{TokenType::PREPRO_ENDIF, ParserElementType::PREPRO_ENDIF},

		{TokenType::SETTARGET, ParserElementType::SETTARGET},
		{TokenType::BEGINREGION, ParserElementType::BEGINREGION},
		{TokenType::ENDREGION, ParserElementType::ENDREGION},

		{TokenType::OPTIMIZE, ParserElementType::OPTIMIZE},

		{TokenType::AMINOS, ParserElementType::AMINOS},
		{TokenType::DNA, ParserElementType::DNA},

		{TokenType::BEGIN, ParserElementType::BEGIN},
		{TokenType::END, ParserElementType::END},

		{TokenType::FROM, ParserElementType::FROM},
		{TokenType::FOR, ParserElementType::FOR},

		{TokenType::DEFINESEQUENCE, ParserElementType::DEFINESEQUENCE},
		{TokenType::FORWARD, ParserElementType::FORWARD},
		{TokenType::FORWARDSEQ, ParserElementType::FORWARDSEQ},
		{TokenType::DEFOP, ParserElementType::DEFOP},
		{TokenType::STATOP, ParserElementType::STATOP},
		{TokenType::FORWARDOP, ParserElementType::FORWARDOP},
		{TokenType::INNERCODE, ParserElementType::INNERCODE},
		{TokenType::CALLOP, ParserElementType::CALLOP},

		{TokenType::DEFPROMOTER, ParserElementType::DEFPROMOTER},
		{TokenType::CALLPROMOTER, ParserElementType::CALLPROMOTER},
		{TokenType::DEFGENE, ParserElementType::DEFGENE},

		{TokenType::UNKNOWN, ParserElementType::UNKNOWN},

		{TokenType::NAMESPACE, ParserElementType::NAMESPACE},
		{TokenType::CREATENAMESPACE, ParserElementType::CREATENAMESPACE},

		//Bool stuff
		{TokenType::IF, ParserElementType::IF},
		{TokenType::BOOL, ParserElementType::BOOL},
		{TokenType::EQUALS, ParserElementType::EQUALS},
		{TokenType::AND, ParserElementType::AND},
		{TokenType::OR, ParserElementType::OR},
		{TokenType::NOT, ParserElementType::NOT},
		{TokenType::NAND, ParserElementType::NAND},
		{TokenType::NOR, ParserElementType::NOR},

		{TokenType::BOOL_TRUE, ParserElementType::BOOL_TRUE},
		{TokenType::BOOL_FALSE, ParserElementType::BOOL_FALSE},

		{TokenType::RPAREN, ParserElementType::RPAREN},
		{TokenType::LPAREN, ParserElementType::LPAREN},
		{TokenType::COMMA, ParserElementType::COMMA},

		//Type stuff
		{TokenType::ASSIGNTYPE, ParserElementType::ASSIGNTYPE},
		{TokenType::TYPEDEF, ParserElementType::TYPEDEF},
		{TokenType::INHERITS, ParserElementType::INHERITS},

		{TokenType::OPERATOR, ParserElementType::OPERATOR},
	};

	//All tokens will be stored in a unique_ptr<void>. This is because we might deal with a bunch of different types of tokens, but we can 
	//hardcode their types when they're accessed
	struct ParserElement
	{
		ParserElement(Token* tok)
		{
			type = tokenToParserElementType[tok->Type];
			value = std::make_unique<Token*>(tok);
		}
		
		ParserElementType type;
		std::unique_ptr<void> value;
	};
}

std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::Token*>& tokens);
std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::ParserElement*>& elems);
std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::ParserElement*>& elems, std::unordered_map<GenParser::ParserElementType, std::vector<size_t>*>& elemLocations);