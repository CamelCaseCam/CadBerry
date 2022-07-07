#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"
#include "GIL/SaveFunctions.h"
#include "GIL/GILException.h"

namespace GIL
{
	std::vector<Parser::Region> CopiedRegions;
	using namespace Lexer;
	using namespace Parser;

	std::string Empty = "";

	enum class SequenceType : char
	{
		StaticSequence,
		SequenceForward,
		Operator,
		EndOfOperators,
	};

	SequenceType SavedSequence;

	std::vector<GIL::Lexer::Token*> EmptyTokens = {};

	Sequence* CreateSequence(SequenceType sequenceType)
	{
		switch (sequenceType)
		{
		case GIL::SequenceType::StaticSequence:
			return new StaticSequence();
		case GIL::SequenceType::SequenceForward:
			return new SequenceForward();
		case GIL::SequenceType::Operator:
			return new Operator();
		default:
			break;
		}
	}

	void StaticSequence::Save(std::ofstream& OutputFile)    //Save the number of tokens and the tokens to the file
	{
		SavedSequence = SequenceType::StaticSequence;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

		//Save the sequence typename
		SaveString(this->SeqType->TypeName, OutputFile);
		
		//Save the parameter index mapping
		int len = this->ParamIdx2Name.size();
		OutputFile.write((char*)&len, sizeof(int));
		for (int i = 0; i < len; i++)
		{
			SaveString(this->ParamIdx2Name[i], OutputFile);
		}

		//Save the parameter types
		len = this->ParameterTypes.size();
		OutputFile.write((char*)&len, sizeof(int));
		for (auto paramtype : this->ParameterTypes)
		{
			SaveString(paramtype.first, OutputFile);
			SaveString(paramtype.second->TypeName, OutputFile);
		}
		

		//Save the Tokens
		int NumTokens = this->Tokens.size();
		OutputFile.write((char*)&NumTokens, sizeof(int));

		for (GIL::Lexer::Token* t : this->Tokens)
		{
			t->Save(OutputFile);
		}
	}

	void StaticSequence::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		//Load the sequence typename
		std::string SeqTypeName;
		LoadStringFromFile(SeqTypeName, InputFile);
		this->SeqType = Proj->Types[Proj->AllocType(SeqTypeName)];
		
		//Load the parameter index mapping
		int len;
		InputFile.read((char*)&len, sizeof(int));
		this->ParamIdx2Name.reserve(len);
		
		for (int i = 0; i < len; i++)
		{
			std::string ParamName;
			LoadStringFromFile(ParamName, InputFile);
			this->ParamIdx2Name.push_back(ParamName);
		}

		//Load the parameter types
		InputFile.read((char*)&len, sizeof(int));
		for (int i = 0; i < len; ++i)
		{
			std::string ParamName;
			LoadStringFromFile(ParamName, InputFile);
			std::string ParamTypeName;
			LoadStringFromFile(ParamTypeName, InputFile);
			this->ParameterTypes[ParamName] = Proj->Types[Proj->AllocType(ParamTypeName)];
		}
		
		
		
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

	std::pair<std::vector<Parser::Region>, std::string> StaticSequence::Get(Parser::Project* Proj, std::map<std::string, Param>& Params)
	{
		if (CurrentSequenceCallDepth > MAX_SEQUENCE_CALL_DEPTH)
		{
			CDB_BuildError("Fatal error: sequence reached max call depth. You probably passed a sequence itself as one of its parameters");
			throw GILException();
		}
		++CurrentSequenceCallDepth;

		std::vector<Region> OutputRegions;
		std::string OutputCode;

		//Compile the sequence and cache the results
		if (!this->IsCompiled)
		{

			for (int i = 0; i < Tokens.size(); ++i)
			{
				int NumTokens = 0;
				//Count until a parameter is reached
				for (i; i < Tokens.size(); ++i)
				{
					if (Tokens[i]->TokenType == LexerToken::PARAM)
						break;
					++NumTokens;
				}

				//Extract the tokens into a vector
				std::vector<Token*> ToBeCompiled;
				ToBeCompiled.reserve(NumTokens);
				for (NumTokens; NumTokens > 0; --NumTokens)
					ToBeCompiled.push_back(this->Tokens[i - NumTokens]);

				CachedSequenceChunk Chunk;
				Chunk.SeqIdx = i;
				//Now compile the chunk
				auto output = Compiler::Compile(Proj, &ToBeCompiled);
				Chunk.Regions = output.first;
				Chunk.Code = output.second;
				this->SequenceCache.push_back(Chunk);

				OutputRegions.insert(OutputRegions.end(), output.first.begin(), output.first.end());
				OutputCode += output.second; 

				if (i == Tokens.size())
					return { OutputRegions, OutputCode };

				//Now compile the sequence
				if (!Params.contains(this->Tokens[i]->Value))
				{
					CDB_BuildError("Parameter \"{0}\" was not passed", this->Tokens[i]->Value);
					continue;
				}
				std::string& ParamName = this->Tokens[i]->Value;
				Param parameter = Params[ParamName];

				//Something important to note, type1->IsOfType(type2) may not be equal to type2->IsOfType(type1) because of inheritance
				if (!this->GetParameterType(ParamName)->IsOfType(parameter.type, false))
				{
					CDB_BuildError("Type mismatch: type \"{0}\" cannot be passed as parameter of type \"{1}\"", parameter.type->TypeName, this->ParameterTypes[ParamName]->TypeName);
					continue;
				}

				int OldIndex = i;
				++i;
				auto SeqParams = Compiler::GetParams(Proj, this->Tokens, i, parameter.Seq->ParamIdx2Name, &Params);
				if (SeqParams.size() == 0)
					i = OldIndex;

				output = parameter.Seq->Get(parameter.SourceProj, SeqParams);
				OutputRegions.insert(OutputRegions.end(), output.first.begin(), output.first.end());
				OutputCode += output.second;
			}
			return { OutputRegions, OutputCode };
		}

		//Load the cached results
		for (CachedSequenceChunk& chunk : this->SequenceCache)
		{
			//Add the unchanging results
			OutputRegions.insert(OutputRegions.end(), chunk.Regions.begin(), chunk.Regions.end());
			OutputCode += chunk.Code;

			if (chunk.SeqIdx == this->Tokens.size() || chunk.SeqIdx == -1)
				return { OutputRegions, OutputCode };

			//Compile the parameter
			if (!Params.contains(this->Tokens[chunk.SeqIdx]->Value))
			{
				CDB_BuildError("Parameter \"{0}\" was not passed", this->Tokens[chunk.SeqIdx]->Value);
				continue;
			}
			std::string& ParamName = this->Tokens[chunk.SeqIdx]->Value;
			Param parameter = Params[ParamName];

			int uselessInt = chunk.SeqIdx + 1;
			auto SeqParams = Compiler::GetParams(Proj, this->Tokens, uselessInt, parameter.Seq->ParamIdx2Name, &Params);

			auto output = parameter.Seq->Get(Proj, SeqParams);
			OutputRegions.insert(OutputRegions.end(), output.first.begin(), output.first.end());
			OutputCode += output.second;
		}
		return { OutputRegions, OutputCode };
	}

	Sequence* Sequence::LoadSequence(std::ifstream& InputFile, Parser::Project* Proj)
	{
		InputFile.read((char*)&SavedSequence, sizeof(SequenceType));

		Sequence* NewSequence = CreateSequence(SavedSequence);
		NewSequence->Load(InputFile, Proj);
		return NewSequence;
	}
	
	std::pair<std::vector<Parser::Region>, std::string> SequenceForward::Get(Parser::Project* Proj, std::map<std::string, Param>& Params)
	{
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		return this->DestinationSequence->Get(Proj, Params);
	}

	std::vector<GIL::Lexer::Token*>& SequenceForward::GetTokens()
	{
		if (this->DestinationSequence == nullptr)
		{
			return EmptyTokens;
		}
		return this->DestinationSequence->GetTokens();
	}	

	void SequenceForward::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::SequenceForward;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

		SaveString(this->DestinationName, OutputFile);
	}
	
	void SequenceForward::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		LoadStringFromFile(this->DestinationName, InputFile);
	}


	bool Sequence::TypesMatch(std::map<std::string, Param>& Params)
	{
		for (auto param : Params)
		{
			//Any untyped parameters should be given the "any" type
			if (!this->ParameterTypes.contains(param.first))
			{
				this->ParameterTypes[param.first] = &Type::any;
				continue;
			}

			if (!param.second.type->IsOfType(this->ParameterTypes[param.first], false))
				return false;
		}
		return true;
	}	

	std::pair<std::vector<Parser::Region>, std::string> Operator::Get(Parser::Project* Proj, std::map<std::string, Param>& Params)
	{
		//First check if the parameters match
		if (!this->TypesMatch(Params))
		{
			if (AlternateImplementation == nullptr)
			{
				CDB_BuildError("Operator forwarding to sequence {0} was called on incompatable types, no alternate implementation(s) found", this->DestinationName);
				return { {}, "" };
			}

			return AlternateImplementation->Get(Proj, Params);
		}
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		return this->DestinationSequence->Get(Proj, Params);
	}

	void Operator::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::Operator;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

		SaveString(this->DestinationName, OutputFile);

		//Now recursively save the operators
		if (this->AlternateImplementation != nullptr)
		{
			AlternateImplementation->Save(OutputFile);
		}
		else
		{
			//Write the signal that there aren't any more operators
			SavedSequence = SequenceType::EndOfOperators;
			OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));
		}
	}

	void Operator::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		//Load the forward
		LoadStringFromFile(this->DestinationName, InputFile);

		//Recursively load the alternate implementations
		InputFile.read((char*)&SavedSequence, sizeof(SequenceType));
		if (SavedSequence == SequenceType::Operator)
		{
			this->AlternateImplementation = new Operator();
			this->AlternateImplementation->Load(InputFile, Proj);
		}
		else
		{
			this->AlternateImplementation = nullptr;
		}
	}

	Operator* Operator::GetLastImplementation()
	{
		if (this->AlternateImplementation == nullptr)
			return this;
		else return this->AlternateImplementation->GetLastImplementation();
	}
}