#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"
#include "GIL/SaveFunctions.h"

namespace GIL
{
	std::vector<Parser::Region> CopiedRegions;

	enum class SequenceType : char
	{
		StaticSequence,
		SequenceForward,
	};

	SequenceType SavedSequence;

	Sequence* CreateSequence(SequenceType sequenceType)
	{
		switch (sequenceType)
		{
		case GIL::SequenceType::StaticSequence:
			return new StaticSequence();
		case GIL::SequenceType::SequenceForward:
			return new SequenceForward();
		default:
			break;
		}
	}

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
		SavedSequence = SequenceType::StaticSequence;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

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

	Sequence* Sequence::LoadSequence(std::ifstream& InputFile)
	{
		InputFile.read((char*)&SavedSequence, sizeof(SequenceType));

		Sequence* NewSequence = CreateSequence(SavedSequence);
		NewSequence->Load(InputFile);
		return NewSequence;
	}

	std::vector<Parser::Region>* SequenceForward::GetRegions(Parser::Project* Proj)
	{
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		return this->DestinationSequence->GetRegions(Proj);
	}

	std::string* SequenceForward::GetCode(Parser::Project* Proj)
	{
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		return this->DestinationSequence->GetCode(Proj);
	}
	
	void SequenceForward::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::SequenceForward;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

		SaveString(this->DestinationName, OutputFile);
	}
	
	void SequenceForward::Load(std::ifstream& InputFile)
	{
		LoadStringFromFile(this->DestinationName, InputFile);
	}
}