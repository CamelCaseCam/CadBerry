#include <gilpch.h>
#include "Sequence.h"
#include "GIL/Compiler/Compiler.h"
#include "GIL/Compiler/CompilerFunctions.h"
#include "GIL/SaveFunctions.h"
#include "GIL/GILException.h"
#include "GIL/Errors.h"

#include "GIL/Parser/Project.h"

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
				//InnerCode is allowed to be defined implicitly
				if (!Params.contains(p->ParamName) && p->ParamName != "InnerCode")
				{
					CDB_BuildError(ERROR_035, p->ParamName, p->pos.Line);
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
		auto seq = Proj->GetSeqFromNamespace(this->DestinationName, this->Location.Namespaces, 0, &Compiler::Modules);
		if (seq.first != nullptr)
		{
			this->DestinationSequence = seq;
			this->ParameterTypes = this->DestinationSequence.first->ParameterTypes;
			this->ParamIdx2Name = this->DestinationSequence.first->ParamIdx2Name;
			this->SeqType = this->DestinationSequence.first->SeqType;
		}
		else
		{
			CDB_BuildError(ERROR_034, this->DestinationName);
		}
	}
	
#define SequenceReturn(val) --CurrentSequenceCallDepth; return val
	void DynamicSequence::Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& Context)
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
			(*SequenceContext.Nodes)[SequenceContext.NodeIdx]->Compile(SequenceContext, Proj);
		}
		--CurrentSequenceCallDepth;
	}

	void Sequence::SaveBaseSequence(std::ofstream& OutputFile)
	{
		//Save sequence type
		this->SeqType->Save(OutputFile);
		
		SaveStringVector(this->ParamIdx2Name, OutputFile);
		
		//Save parameter types
		size_t NumParams = this->ParameterTypes.size();
		OutputFile.write((char*)&NumParams, sizeof(size_t));
		for (auto param : this->ParameterTypes)
		{
			SaveString(param.first, OutputFile);
			SaveString(param.second->TypeName, OutputFile);
		}
	}
	
	void Sequence::LoadBaseSequence(std::ifstream& InputFile, Parser::Project* Proj)
	{
		this->SeqType = Type().Load(InputFile, Proj);
		
		LoadStringVectorFromFile(this->ParamIdx2Name, InputFile);
		
		size_t NumParams = -1;
		InputFile.read((char*)&NumParams, sizeof(size_t));
		for (int i = 0; i < NumParams; ++i)
		{
			std::string ParamName;
			LoadStringFromFile(ParamName, InputFile);
			std::string TypeName;
			LoadStringFromFile(TypeName, InputFile);
			uint16_t ID = Proj->AllocType(TypeName);
			this->ParameterTypes[ParamName] = Proj->GetTypeByID(ID);
		}
	}

	void DynamicSequence::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::DynamicSequence;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));
		this->SaveBaseSequence(OutputFile);

		SaveStringVector(this->ActiveDistributions, OutputFile);
		
		SaveSize(this->Nodes.size(), OutputFile);
		for (AST_Node* n : this->Nodes)
		{
			AST_Node::SaveNode(n, OutputFile);
		}
	}

	void DynamicSequence::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		this->LoadBaseSequence(InputFile, Proj);
		LoadStringVectorFromFile(this->ActiveDistributions, InputFile);
		
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
	
	void SequenceForward::Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& Context)
	{
		if (this->DestinationSequence.first == nullptr)
		{
			this->DestinationSequence = Proj->GetSeqFromNamespace(this->DestinationName, this->Location.Namespaces, 0, &Compiler::Modules);
		}
		this->DestinationSequence.first->Get(this->DestinationSequence.second, Params, Context);
	}
	void* SequenceForward::Data(Project* Proj)
	{
		if (this->DestinationSequence.first == nullptr)
		{
			this->DestinationSequence = Proj->GetSeqFromNamespace(this->DestinationName, this->Location.Namespaces, 0, &Compiler::Modules);
		}
		return this->DestinationSequence.first->Data(this->DestinationSequence.second);
	}

	std::vector<GIL::Lexer::Token*>& SequenceForward::GetTokens()
	{
		if (this->DestinationSequence.first == nullptr)
		{
			return EmptyTokens;
		}
		return this->DestinationSequence.first->GetTokens();
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
	
	void BoolSequence::Save(std::ofstream& OutputFile)
	{
		SavedSequence = SequenceType::SequenceForward;
		OutputFile.write((char*)&SavedSequence, sizeof(SequenceType));

		
	}

	void BoolSequence::Load(std::ifstream& InputFile, Parser::Project* Proj)
	{
		
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

	void Operator::Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& Context)
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
		if (this->DestinationSequence.first == nullptr)
		{
			this->DestinationSequence = Proj->GetSeqFromNamespace(this->DestinationName, this->Location.Namespaces, 0, &Compiler::Modules);
		}
		this->DestinationSequence.first->Get(this->DestinationSequence.second, Params, Context);
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
		this->SaveBaseSequence(OutputFile);
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
		this->LoadBaseSequence(InputFile, Proj);
	}

	Operator* Operator::GetLastImplementation()
	{
		if (this->AlternateImplementation == nullptr)
			return this;
		else return this->AlternateImplementation->GetLastImplementation();
	}

	void InnerCode::Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context)
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


	void Sequence::Get(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context)
	{
		if (this->ActiveDistributions.size() == 0)
		{
			this->Get_impl(Proj, Params, context);
			return;
		}
		
		if (context.Distribution == nullptr)
			return;
		
		for (std::string& dist : this->ActiveDistributions)
		{
			if (*context.Distribution == dist)
			{
				this->Get_impl(Proj, Params, context);
				return;
			}
		}
	}
}