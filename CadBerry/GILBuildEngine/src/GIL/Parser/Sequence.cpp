#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"

namespace GIL
{
	std::vector<Parser::Region> CopiedRegions;

	std::vector<Parser::Region>* StaticSequence::GetRegions(Parser::Project* Proj)
	{
		if (!this->IsCompiled)    //Only compile the sequence once, on subsequent calls used the cached result
		{
			IsCompiled = true;
			auto output = GIL::Compiler::Compile(Proj, &this->Tokens);
			this->Regions = output.first;
			this->Code = output.second;
		}
		
		CopiedRegions.clear();
		CopiedRegions.reserve(this->Regions.size());
		for (Parser::Region r : this->Regions)
		{
			CopiedRegions.push_back(r);
		}
		return &CopiedRegions;
	}

	std::string* StaticSequence::GetCode(Parser::Project* Proj)
	{
		if (!this->IsCompiled)    //Only compile the sequence once, on subsequent calls used the cached result
		{
			auto output = GIL::Compiler::Compile(Proj, &this->Tokens);
			this->Regions = output.first;
			this->Code = output.second;
			IsCompiled = true;
		}
		return &this->Code;
	}

	void StaticSequence::Save(std::ofstream& OutputFile)    //Save the number of tokens and the tokens to the file
	{
		int NumTokens = this->Tokens.size();
		OutputFile.write((char*)&NumTokens, sizeof(int));

		for (GIL::Lexer::Token* t : this->Tokens)
		{
			t->Save(OutputFile);
		}
	}

	void StaticSequence::Load(std::ifstream& InputFile)
	{
		int NumTokens;
		InputFile.read((char*)&NumTokens, sizeof(int));
		this->Tokens.reserve(NumTokens);

		for (int i = 0; i < NumTokens; ++i)
		{
			this->Tokens.push_back(GIL::Lexer::Token::Load(InputFile));
		}
	}

	StaticSequence::~StaticSequence()
	{
		for (GIL::Lexer::Token* t : this->Tokens)
		{
			GIL::Lexer::Token::SafeDelete(t);    //Only deletes unique tokens
		}
	}
}