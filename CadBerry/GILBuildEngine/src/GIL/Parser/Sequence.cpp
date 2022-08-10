#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"
#include "GIL/SaveFunctions.h"
#include "GIL/GILException.h"
#include "GIL/Errors.h"

namespace GIL
{
	std::vector<Parser::Region> CopiedRegions;
	using namespace Lexer;
	using namespace Parser;
	using namespace Compiler;

	std::string Empty = "";

	enum class SequenceType : char
	{
		StaticSequence,
		SequenceForward,
		DynamicSequence,
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
			CDB_BuildError("Static sequences aren't currently supported. Terminating compilation");
			throw GILException();
		case GIL::SequenceType::SequenceForward:
			return new SequenceForward();
		case GIL::SequenceType::DynamicSequence:
			return new DynamicSequence();
		case GIL::SequenceType::Operator:
			return new Operator();
		default:
			break;
		}
	}


	//Dynamic sequence stuff
	DynamicSequence::~DynamicSequence()
	{
		//Delete all the AST nodes
		for (AST_Node* n : this->Nodes)
		{
			delete n;
		}
	}

	
	//This can definitely be optimized, but it works
	void ConvertParamNodes(Parser::Project* Proj, std::map<std::string, Param>& Params, CompilerContext& Context, std::vector<AST_Node*>& Nodes)
	{
		for (AST_Node* n : Nodes)
		{
			std::string NodeName = n->GetName();
			if (NodeName == "UseParam")
			{
				UseParam* p = (UseParam*)n;
				if (!Params.contains(p->ParamName))
				{
					CDB_BuildError("Fatal error: parameter " + p->ParamName + " not found in parameter list");
					throw GILException();
				}
				else
				{
					//Set the parameter's contents. This should let us use parameters in InnerCode
					p->m_Param = &Params[p->ParamName];
				}
			}
			else if (NodeName == "From")
				ConvertParamNodes(Proj, Params, Context, ((From*)n)->Body);
			else if (NodeName == "For")
				ConvertParamNodes(Proj, Params, Context, ((For*)n)->Body);
			else if (NodeName == "CallOperation")
				ConvertParamNodes(Proj, Params, Context, ((CallOperation*)n)->InnerNodes);
			else if (NodeName == "Prepro_If")
				ConvertParamNodes(Proj, Params, Context, ((Prepro_If*)n)->Body);
			else if (NodeName == "Prepro_Else")
				ConvertParamNodes(Proj, Params, Context, ((Prepro_Else*)n)->Body);
		}
	}

	void SequenceForward::CompileTimeInit(Parser::Project* Proj)
	{
		if (Proj->Sequences.contains(this->DestinationName))
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
			this->ParameterTypes = this->DestinationSequence->ParameterTypes;
			this->ParamIdx2Name = this->DestinationSequence->ParamIdx2Name;
			this->SeqType = this->DestinationSequence->SeqType;
		}
		else
		{
			CDB_BuildError(ERROR_034, this->DestinationName);
		}
	}
	
#define SequenceReturn(val) --CurrentSequenceCallDepth; return val
	void DynamicSequence::Get(Parser::Project* Proj, std::map<std::string, Param>& Params, CompilerContext& Context)
	{
		if (CurrentSequenceCallDepth > MAX_SEQUENCE_CALL_DEPTH)
		{
			CDB_BuildError("Fatal error: sequence reached max call depth. You probably passed a sequence itself as one of its parameters");
			throw GILException();
		}
		++CurrentSequenceCallDepth;
		
		//Make the parameters point to the right sequences
		ConvertParamNodes(Proj, Params, Context, this->Nodes);

		//Create the new compiler context
		CompilerContext SequenceContext = Context;
		SequenceContext.Nodes = &this->Nodes;
		SequenceContext.NodeIdx = 0;
		
		//Loop through the nodes. If a node is a parameter, replace it with the parameter's value.
		for (SequenceContext; SequenceContext.NodeIdx < SequenceContext.Nodes->size(); ++SequenceContext.NodeIdx)
		{
<<<<<<< HEAD

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
=======
			(*SequenceContext.Nodes)[SequenceContext.NodeIdx]->Compile(SequenceContext, Proj);
>>>>>>> parser-ast
		}
		--CurrentSequenceCallDepth;
	}

	void DynamicSequence::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::DynamicSequence;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));
		
		SaveSize(this->Nodes.size(), OutputFile);
		for (AST_Node* n : this->Nodes)
		{
			AST_Node::SaveNode(n, OutputFile);
		}
	}

	void DynamicSequence::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		size_t Size = LoadSizeFromFile(InputFile);
		this->Nodes.reserve(Size);
		for (size_t i = 0; i < Size; ++i)
		{
			this->Nodes.push_back(AST_Node::LoadNode(InputFile, Proj));
		}
	}
	
	
	

	Sequence* Sequence::LoadSequence(std::ifstream& InputFile, Parser::Project* Proj)
	{
		InputFile.read((char*)&SavedSequence, sizeof(SequenceType));

		Sequence* NewSequence = CreateSequence(SavedSequence);
		NewSequence->Load(InputFile, Proj);
		return NewSequence;
	}
	
	void SequenceForward::Get(Parser::Project* Proj, std::map<std::string, Param>& Params, CompilerContext& Context)
	{
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		this->DestinationSequence->Get(Proj, Params, Context);
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

	void Operator::Get(Parser::Project* Proj, std::map<std::string, Param>& Params, CompilerContext& Context)
	{
		//First check if the parameters match
		if (!this->TypesMatch(Params))
		{
			if (AlternateImplementation == nullptr)
			{
				CDB_BuildError("Operator forwarding to sequence {0} was called on incompatable types, no alternate implementation(s) found", this->DestinationName);
				return;
			}

			AlternateImplementation->Get(Proj, Params, Context);
		}
		if (this->DestinationSequence == nullptr)
		{
			this->DestinationSequence = Proj->Sequences[this->DestinationName];
		}
		this->DestinationSequence->Get(Proj, Params, Context);
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

	void InnerCode::Get(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context)
	{
		//Add the regions to the context
		context.OutputRegions->reserve(context.OutputRegions->size() + this->m_InnerCode.first.size());
		
		for (auto region : this->m_InnerCode.first)
		{
			region.Add(context.OutputString->length());
			context.OutputRegions->push_back(std::move(region));
		}
		
		*context.OutputString += this->m_InnerCode.second;
	}
}