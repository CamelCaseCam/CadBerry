//Handmade generational parser made to create the algorithm

//Prologue
#include <gilpch.h>
using namespace GIL::Parser;

#define mv(x) std::move(x)
#define vec(x) std::vector< x >
//End prologue

#include "gram.h"
#include <functional>

using namespace GenParser;


std::vector<ParserElementType> types = {
	//Tokens
	ParserElementType::IMPORT,
	ParserElementType::STRING,
	ParserElementType::USING,
	ParserElementType::IDENT,
	ParserElementType::NEWLINE,
	ParserElementType::COMMENT,
	ParserElementType::SETATTR,
	ParserElementType::PARAM,

	ParserElementType::CREATEVAR,
	ParserElementType::INC,
	ParserElementType::DEC,
	ParserElementType::PREPRO_IF,
	ParserElementType::PREPRO_ELSE,
	ParserElementType::PREPRO_ENDIF,

	ParserElementType::SETTARGET,
	ParserElementType::BEGINREGION,
	ParserElementType::ENDREGION,

	ParserElementType::OPTIMIZE,

	ParserElementType::AMINOS,
	ParserElementType::DNA,

	ParserElementType::BEGIN,
	ParserElementType::END,

	ParserElementType::FROM,
	ParserElementType::FOR,

	ParserElementType::DEFINESEQUENCE,
	ParserElementType::FORWARD,
	ParserElementType::FORWARDSEQ,
	ParserElementType::DEFOP,
	ParserElementType::STATOP,
	ParserElementType::FORWARDOP,
	ParserElementType::INNERCODE,
	ParserElementType::CALLOP,

	ParserElementType::DEFPROMOTER,
	ParserElementType::CALLPROMOTER,
	ParserElementType::DEFGENE,

	ParserElementType::UNKNOWN,

	ParserElementType::NAMESPACE,
	ParserElementType::CREATENAMESPACE,

		//Bool stuff
	ParserElementType::IF,
	ParserElementType::BOOL,
	ParserElementType::EQUALS,
	ParserElementType::AND,
	ParserElementType::OR,
	ParserElementType::NOT,
	ParserElementType::NAND,
	ParserElementType::NOR,

	ParserElementType::BOOL_TRUE,
	ParserElementType::BOOL_FALSE,

	ParserElementType::RPAREN,
	ParserElementType::LPAREN,
	ParserElementType::COMMA,

		//Type stuff
	ParserElementType::ASSIGNTYPE,
	ParserElementType::TYPEDEF,
	ParserElementType::INHERITS,

	ParserElementType::OPERATOR,

		//Nodes
	ParserElementType::LexerToken,
	ParserElementType::SET_TARGET,
	ParserElementType::SET_ATTR,
	ParserElementType::SET_VAR,
	ParserElementType::BEGIN_REGION,
	ParserElementType::END_REGION,
	ParserElementType::INC_VAR,
	ParserElementType::DEC_VAR,
	ParserElementType::P_PARAM,
	ParserElementType::P_IMPORT,
	ParserElementType::P_USING,
	ParserElementType::P_OPERATOR,
	ParserElementType::P_FORWARD,
	ParserElementType::CALL_PARAM,
	ParserElementType::NAMESPACE_ACCESSION,
	ParserElementType::DNA_LITERAL,
	ParserElementType::AMINO_ACID_LITERAL,
	ParserElementType::P_TYPEDEF,
	ParserElementType::P_INHERITS,
	ParserElementType::COMMENT,
	ParserElementType::PARAM_DEFINITION_GROUP,
	ParserElementType::CALL_PARAMS,
	ParserElementType::CALL_SEQUENCE,
	ParserElementType::P_CALLOP,
	ParserElementType::STATEMENT,
	ParserElementType::P_PREPRO_IF,
	ParserElementType::P_PREPRO_ELSE,
	ParserElementType::P_FROM,
	ParserElementType::P_FOR,
	ParserElementType::DEFINE_SEQUENCE,
	ParserElementType::DEFINE_OPERATION,
	ParserElementType::CREATE_NAMESPACE,
};
extern std::unordered_map<ParserElementType, std::vector<std::function<bool(std::vector<GenParser::ParserElement*>& elems, size_t i, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)>>> rules;


std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::Token*>& tokens)
{
	std::vector<GenParser::ParserElement*> elements;
	elements.reserve(tokens.size());
	for (auto token : tokens)
	{
		elements.push_back(new GenParser::ParserElement(token));
	}
	return Parse(elements);
}

std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::ParserElement*>& elems)
{
	//Create the map of types and token locations
	std::unordered_map<ParserElementType, std::vector<size_t>*> TypeLocations;
	for (ParserElementType type : types)
	{
		TypeLocations[type] = new std::vector<size_t>();
	}
	for (size_t i = 0; i < elems.size(); i++)
	{
		TypeLocations[elems[i]->type]->push_back(i);
	}

	std::vector<GenParser::ParserElement*> output;
	//Parse the tokens
	try
	{
		output = Parse(elems, TypeLocations);
	} catch (std::exception e)
	{
		//Delete the map
		for (auto kvp : TypeLocations)
		{
			delete kvp.second;
		}
		throw;
	}
	
	//Delete the map
	for (auto kvp : TypeLocations)
	{
		delete kvp.second;
	}
	return output;
}

//end is exclusive
void RemoveUsedTokens(std::vector<GenParser::ParserElement*>& elems, size_t mapidx, size_t start, size_t end, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)
{
	for (int i = start; i < end; ++i)
	{
		//Remove the token from the map
		auto& vec = *elemLocations[elems[i]->type];
		vec.erase(vec.begin() + mapidx);

		delete elems[i];
	}
	elems.erase(elems.begin() + start, elems.begin() + end);
}


//SETTARGET IDENT:str
#define SET_TARGET_1_LEN 2
bool SET_TARGET_1(std::vector<GenParser::ParserElement*>& elems, size_t i, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)
{
	//Execute the rule
	size_t index = elemLocations[ParserElementType::SETTARGET]->at(i);
	if (index + SET_TARGET_1_LEN > elems.size())
	{
		return false;
	}

	size_t elemIndex = index + 1;
	if (elems[elemIndex]->type != ParserElementType::IDENT)
	{
		return false;
	}
	//Get elemIndex into the variable
	TokenValue parsevar_str = ((Token*)elems[elemIndex]->value.get())->Value;

	ParserElement* NewElem = elems[index];
	NewElem->type = ParserElementType::SET_TARGET;
	//Execute the reduction code
	auto val = new SetTarget(mv(parsevar_str));
	NewElem->value.reset(val);

	//Now remove consumed tokens
	RemoveUsedTokens(elems, i, index + 1, index + 2, elemLocations);

	//Add this to the token location map
	size_t thisIdx = elemLocations[ParserElementType::SET_TARGET]->size();
	elemLocations[ParserElementType::SET_TARGET]->push_back(index);
	
	
	//Finally, check if any rules can be applied again
	for (auto func : rules[ParserElementType::SET_TARGET])
	{
		if (func(elems, thisIdx, elemLocations))
			break;
	}
}


//SETTARGET STRING:str
#define SET_TARGET_2_LEN 2
bool SET_TARGET_2(std::vector<GenParser::ParserElement*>& elems, size_t i, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)
{
	//Execute the rule
	size_t index = elemLocations[ParserElementType::SETTARGET]->at(i);
	if (index + SET_TARGET_2_LEN > elems.size())
	{
		return false;
	}

	size_t elemIndex = index + 1;
	if (elems[elemIndex]->type != ParserElementType::STRING)
	{
		return false;
	}
	//Get elemIndex into the variable
	TokenValue parsevar_str = ((Token*)elems[elemIndex]->value.get())->Value;

	ParserElement* NewElem = elems[index];
	NewElem->type = ParserElementType::SET_TARGET;
	//Execute the reduction code
	auto val = new SetTarget(mv(parsevar_str));
	NewElem->value.reset(val);

	//Now remove consumed tokens
	RemoveUsedTokens(elems, i, index + 1, index + 2, elemLocations);

	//Add this to the token location map
	size_t thisIdx = elemLocations[ParserElementType::SET_TARGET]->size();
	elemLocations[ParserElementType::SET_TARGET]->push_back(index);


	//Finally, check if any rules can be applied again
	for (auto func : rules[ParserElementType::SET_TARGET])
	{
		if (func(elems, thisIdx, elemLocations))
			break;
	}
}






std::unordered_map<ParserElementType, std::vector<std::function<bool(std::vector<GenParser::ParserElement*>& elems, size_t i, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)>>> rules =
{
	{ParserElementType::SETTARGET, {SET_TARGET_1, SET_TARGET_2}},
};









std::vector<GenParser::ParserElement*> Parse(std::vector<GenParser::ParserElement*>& elems, std::unordered_map<ParserElementType, std::vector<size_t>*>& elemLocations)
{
	//Finally, the actual parsing method

	//_____________________________________________________________________________________
	// GENERATION 1
	//_____________________________________________________________________________________

	//SET_TARGET_1 
	for (size_t i = 0; i < elemLocations[ParserElementType::SETTARGET]->size(); i++)
	{
		SET_TARGET_1(elems, i, elemLocations);
	}

	//SET_TARGET_2
	for (size_t i = 0; i < elemLocations[ParserElementType::SETTARGET]->size(); i++)
	{
		SET_TARGET_2(elems, i, elemLocations);
	}
}