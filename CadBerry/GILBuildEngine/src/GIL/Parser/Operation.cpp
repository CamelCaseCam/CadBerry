#include "gilpch.h"
#include "Operation.h"
#include "GIL/Compiler/Compiler.h"
#include "GIL/Lexer/LexerTokens.h"
#include "GIL/Lexer/Token.h"

namespace GIL
{
	using namespace Lexer;
	using namespace Parser;
	using namespace Compiler;

	StaticOperation::~StaticOperation()
	{
		for (Parser::Region* r : Regions)
			delete r;
	}

	std::pair<std::vector<Parser::Region>, std::string> StaticOperation::Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj)
	{
		//TODO: Implement static operations
		return { {}, "" };
	}

	DynamicOperation::~DynamicOperation()
	{
		for (Lexer::Token* t : tokens)
			t->SafeDelete(t);
	}

	std::pair<std::vector<Parser::Region>, std::string> DynamicOperation::Get(std::vector<Lexer::Token*> InnerTokens, Parser::Project* Proj)
	{
		//First we need to copy the tokens and innertokens into a vector
		int NumTokens = (this->tokens.size() - this->NumInnerCode) + (InnerTokens.size() * this->NumInnerCode);
		std::vector<Token*> Tokens;
		Tokens.reserve(NumTokens);

		for (Token* t : this->tokens)
		{
			if (t->TokenType == LexerToken::INNERCODE)
			{
				for (Token* it : InnerTokens)
				{
					Tokens.push_back(it);
				}
			}
			else
			{
				Tokens.push_back(t);
			}
		}

		//Now we compile the result and return it
		return Compile(Proj, &Tokens);
	}

	void DynamicOperation::Save(std::ofstream& OutputFile)
	{
		OutputFile.write((char*)&this->NumInnerCode, sizeof(int));
		int NumTokens = this->tokens.size();
		OutputFile.write((char*)&NumTokens, sizeof(int));

		for (Token* t : this->tokens)
		{
			t->Save(OutputFile);
		}
	}
	void DynamicOperation::Load(std::ifstream& InputFile)
	{
		InputFile.read((char*)&this->NumInnerCode, sizeof(int));

		int NumTokens;
		InputFile.read((char*)&NumTokens, sizeof(int));
		this->tokens.reserve(NumTokens);

		for (int i = 0; i < NumTokens; ++i)
		{
			this->tokens.push_back(Token::Load(InputFile));
		}
	}
}