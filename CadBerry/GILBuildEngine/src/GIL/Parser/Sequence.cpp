#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"

namespace GIL
{
	std::vector<Parser::Region>* StaticSequence::GetRegions(Parser::Project* Proj)
	{
		if (!this->IsCompiled)
		{
			IsCompiled = true;
			auto output = GIL::Compiler::Compile(Proj, &this->Tokens);
			this->Regions = output.first;
			this->Code = output.second;
		}
		return &this->Regions;
	}

	std::string* StaticSequence::GetCode(Parser::Project* Proj)
	{
		if (!this->IsCompiled)
		{
			auto output = GIL::Compiler::Compile(Proj, &this->Tokens);
			this->Regions = output.first;
			this->Code = output.second;
			IsCompiled = true;
		}
		return &this->Code;
	}

	void StaticSequence::Save(std::ofstream& OutputFile)
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
			GIL::Lexer::Token::SafeDelete(t);
		}
	}
}