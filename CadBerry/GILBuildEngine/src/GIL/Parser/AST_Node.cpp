#include "gilpch.h"
#include "AST_Node.h"
#include "GIL/Lexer/Token.h"
#include "GIL/Parser/Project.h"
#include "GIL/Compiler/CompilerFunctions.h"
#include "GIL/SaveFunctions.h"

#include "GIL/GILException.h"

namespace GIL
{
	using namespace Compiler;
	using namespace Lexer;
	namespace Parser
	{
		std::unordered_map<std::string, VariableType> VariableTypes = {
			{ "str", VariableType::str },
			{ "num", VariableType::num }
		};


		//This MUST be called for nodes loaded using LoadNode, otherwise the node's type information won't be saved
		void AST_Node::SaveNode(AST_Node* Node, std::ofstream& OutputFile)
		{
			SaveString(Node->GetName(), OutputFile);

			//Save position
			OutputFile.write((char*)&Node->pos.Line, sizeof(size_t));
			OutputFile.write((char*)&Node->pos.Column, sizeof(size_t));
			
			if (_Reflectable_AST_Node_Map.contains(Node->GetName()))
			{
				Node->Save(OutputFile);
			}
		}
		
		AST_Node* AST_Node::LoadNode(std::ifstream& InputFile, Parser::Project* Proj)
		{
			std::string NodeType;
			LoadStringFromFile(NodeType, InputFile);
			
			ParserPosition pos;
			InputFile.read((char*)&pos.Line, sizeof(size_t));
			InputFile.read((char*)&pos.Column, sizeof(size_t));
			
			//Create a node of that type
			if (_Reflectable_AST_Node_Map.contains(NodeType))
			{
				AST_Node* Node = _Reflectable_AST_Node_Map[NodeType]();
				Node->pos = pos;

				//Load and return the node
				Node->Load(InputFile, Proj);
				return Node;
			}
			return nullptr;
		}
		
		Call_Params::Call_Params(std::vector<GIL::Lexer::Token*>&& Params)
		{
			this->Params.reserve(Params.size());
			for (auto token : Params)
			{
				this->Params.push_back(std::move(token->Value));
				GIL::Lexer::Token::SafeDelete(token);
			}
			Params.clear();
		}

		void Import::AddToProject(Parser::Project* Proj)
		{
			Proj->Imports.push_back(this->FileName);
		}

		void Using::AddToProject(Parser::Project* Proj)
		{
			Proj->Usings.push_back(this->FileName);
		}

		//TODO: namespaces
		void DefineSequence::AddToProject(Parser::Project* Proj)
		{
			DynamicSequence* Seq = new DynamicSequence(this->Body);
			
			//Set up parameters
			Seq->ParamIdx2Name.reserve(this->m_Params.m_Params.size());
			for (auto param : this->m_Params.m_Params)
			{
				//TODO: fix this - it's just assuming that the type will be created at some point
				Seq->ParameterTypes[param->Name] = Proj->GetTypeByID(Proj->AllocType(param->Type));
				Seq->ParamIdx2Name.push_back(param->Name);
			}

			//Set up sequence type
			Seq->SeqType = Proj->GetTypeByID(Proj->AllocType(this->Type));

			//Set up distributions
			Seq->ActiveDistributions = this->ActiveDistributions;
			
			//Add the sequence to the project
			Proj->Sequences[this->Name] = Seq;
		}
		
		
		void InheritTypedef::AddToProject(Parser::Project* Proj)
		{
			Proj->AddInheritance(Proj->AllocType(this->Type), Proj->AllocType(this->Parent));
		}
		
		void TypedefNode::AddToProject(Parser::Project* Proj)
		{
			//Create the type (this is really the only place AllocType is being used correctly)
			uint16_t TypeID = Proj->AllocType(this->Name);
			
			//Give the type its base type
			Proj->AddInheritance(TypeID, Proj->AllocType(this->Inherits));
		}
		
		void AminoAcidLiteral::Compile(CompilerContext& Context, Project* Project)
		{
			AminosToDNA(*Context.OutputString, this->Literal, Project, *Context.Encoding);
		}
		
		void AminoAcidLiteral::Save(std::ofstream& OutputFile)
		{
			SaveString(this->Literal, OutputFile);
		}
		
		void AminoAcidLiteral::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->Literal, InputFile);
		}

		std::map<std::string, Param> Call_Params::ToParamMap(CompilerContext& Context, Project* Project, std::vector<std::string>& ParamIdx2Name)
		{
			std::map<std::string, Param> Output;
			for (int i = 0; i < this->Params.size(); ++i)
			{
				std::string& param = this->Params[i];
				//Check if this is a parameter
				if (Context.Params != nullptr && Context.Params->contains(param))
				{
					Output[ParamIdx2Name[i]] = (*Context.Params)[param];
					continue;
				}
				
				//Otherwise, make sure it exists
				if (!Project->Sequences.contains(param))
				{
					//We can't recover from this, terminate compilation
					CDB_BuildError("Sequence {0} passed as parameter does not exist", param);
					throw GILException();
				}
				Output[ParamIdx2Name[i]] = Param(Project->Sequences[param], Project, Project->Sequences[param]->SeqType);
			}
			return Output;
		}
		
		void UseParam::Compile(CompilerContext& Context, Project* Project)
		{
			//Get the parameter and treat it like a regular identifier
			if (this->m_Param == nullptr || this->m_Param->Seq == nullptr)
			{
				CDB_BuildError("Parameter \"{0}\" was not passed to sequence", this->ParamName);
				return;
			}
			auto params = this->Params.ToParamMap(Context, Project, this->m_Param->Seq->ParamIdx2Name);
			auto OldParams = Context.Params;
			Context.Params = &params;

			//Now compile the parameter
			this->m_Param->Seq->Get(Project, params, Context);
			
			Context.Params = OldParams;
		}

		void Call_Params::Save(std::ofstream& OutputFile)
		{
			SaveStringVector(this->Params, OutputFile);
		}
		
		void Call_Params::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringVectorFromFile(this->Params, InputFile);
		}
		
		void UseParam::Save(std::ofstream& OutputFile)
		{
			SaveString(this->ParamName, OutputFile);
			this->Params.Save(OutputFile);
		}
		
		void UseParam::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->ParamName, InputFile);
			this->Params.Load(InputFile, Proj);
		}
		
		
		void From::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			std::vector<Region> Regions;
			std::vector<Region> OpenRegions;
			std::string Output;
			
			//Copy the context, but set the output to the output string
			CompilerContext NewContext = context;
			NewContext.OutputRegions = &Regions;
			NewContext.OutputString = &Output;
			NewContext.OpenRegions = &OpenRegions;

			
			//Compile the body
			for (auto& node : this->Body)
			{
				node->Compile(NewContext, Project);
			}

			std::string Aminos;
			std::vector<int> Idxs;
			
			/*
			"from Unoptomized" exists so that you can take a sequence from an organism without a gilEncoding and have it generate a
			sequence that *should* work in your target. If you set the origin as Unoptimized, it will pick the best codons for your
			target. This could cause problems with protein folding, but it'll probably be fine
			*/
			if (this->Origin == "Unoptimized")
			{
				DNA2Aminos(Aminos, Idxs, Output, Project);    //Convert DNA to amino acids
				AminosToDNA(Output, Aminos, Project, *context.Encoding);    //Convert amino acids to DNA optimized for target
			}
			else
			{
				CodonEncoding OriginEncoding(this->Origin);    //Get the organism's codonencoding
				
				DNA2Aminos(Aminos, Idxs, Output, Project, OriginEncoding);    //Convert DNA to amino acids
				AminosIDXToDNA(Output, Aminos, Idxs, Project, *context.Encoding);    //Convert amino acids to DNA optimized for target
			}
			
			//Add the regions to the output
			//Reserve enough space (including region to say the area was translated)
			context.OutputRegions->reserve(context.OutputRegions->size() + Regions.size() + 1);
			
			//Region saying that the area was translated
			Region TranslatedRegion("Translated protein from organism " + this->Origin, context.OutputString->length(),
				context.OutputString->length() + Output.length());
			AddRegionToVector(TranslatedRegion, *context.OutputRegions);

			for (Region& r : Regions)
			{
				if (r.Name == "")
					continue;
				r.Add(context.OutputString->length());
				AddRegionToVector(r, *context.OutputRegions);
			}
			*context.OutputString += Output;
		}
	
		void From::Save(std::ofstream& OutputFile)
		{
			//Save the origin organism
			SaveString(this->Origin, OutputFile);
			
			//Save the number of body nodes
			SaveSize(this->Body.size(), OutputFile);
			//Save the body nodes
			for (auto& node : this->Body)
			{
				SaveNode(node, OutputFile);
			}
		}
		void From::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			//Load the origin organism
			LoadStringFromFile(this->Origin, InputFile);
			
			//Load the number of body nodes
			size_t BodySize = LoadSizeFromFile(InputFile);
			this->Body.reserve(BodySize);
			
			//Load the body nodes
			for (size_t i = 0; i < BodySize; i++)
			{
				AST_Node* output = LoadNode(InputFile, Proj);
				if (output != nullptr)
					this->Body.push_back(output);
			}
		}
		
		void For::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//Load the new encoding and set the context to use it
			CodonEncoding* OldEncoding = context.Encoding;
			CodonEncoding NewEncoding(this->Target);
			context.Encoding = &NewEncoding;
					
			//Compile the body
			for (auto& node : this->Body)
			{
				try
				{
					node->Compile(context, Project);
				}
				catch (GILException e)
				{
					/*
					* GILExceptions generally should only be caught by the compiler, but it is possible that a module could catch an exception
					* thrown in a compilation that it initiated. In this case, we need to make sure the encoding has been reset to the old one before
					* rethrowing the exception. 
					*/
					context.Encoding = OldEncoding;
					throw;
				}
			}
			
			//Set the context back to the old encoding
			context.Encoding = OldEncoding;
		}
		
		void For::Save(std::ofstream& OutputFile)
		{
			//Save the target
			SaveString(this->Target, OutputFile);
			
			//Save the number of body nodes
			SaveSize(this->Body.size(), OutputFile);
			//Save the body nodes
			for (auto& node : this->Body)
			{
				SaveNode(node, OutputFile);
			}
		}
		
		void For::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			//Load the target
			LoadStringFromFile(this->Target, InputFile);
			
			//Load the number of body nodes
			size_t BodySize = LoadSizeFromFile(InputFile);
			this->Body.reserve(BodySize);
			
			//Load the body nodes
			for (size_t i = 0; i < BodySize; i++)
			{
				this->Body.push_back(LoadNode(InputFile, Proj));
			}
		}
		
		//Remember, this is *just* a sequence with an InnerCode parameter
		void DefineOperation::AddToProject(Parser::Project* Proj)
		{
			DynamicSequence* Seq = new DynamicSequence(this->Body);

			//Set up parameters
			Seq->ParamIdx2Name.reserve(this->m_Params.m_Params.size());
			for (auto param : this->m_Params.m_Params)
			{
				//TODO: fix this - it's just assuming that the type will be created at some point
				Seq->ParameterTypes[param->Name] = Proj->GetTypeByID(Proj->AllocType(param->Type));
				Seq->ParamIdx2Name.push_back(param->Name);
			}

			//Set up sequence type
			Seq->SeqType = Proj->GetTypeByID(Proj->AllocType(this->Type));

			//Set up distributions
			Seq->ActiveDistributions = this->ActiveDistributions;

			//Add the sequence to the project
			Proj->Sequences[this->Name] = Seq;
		}
		
		void Forward::AddToProject(Parser::Project* Proj)
		{
			//Create the sequence forward
			SequenceForward* Forward = new SequenceForward();
			Forward->DestinationName = std::move(this->Destination);
			Forward->Location = std::move(this->Location);
			Proj->Sequences[this->Origin] = Forward;
		}
		
		void DefineOperator::AddToProject(Parser::Project* Proj)
		{
			//Check if an alternate version of this operator exits
			Operator* AlternateImplementation = nullptr;
			if (Proj->Operators.contains(this->Name))
				AlternateImplementation = (Operator*)Proj->Operators[this->Name];
			
			Operator* Op = new Operator();
			Op->DestinationName = this->Implementation;
			Op->AlternateImplementation = AlternateImplementation;
			Proj->Operators[this->Name] = Op;
		}
		
		
		/*
		* Operators are treated as follows:
		* 1. A unary operator will be called on the sequence after it if it exists, or otherwise, the sequence before it
		* 2. A binary operator will be called on the sequences before and after it. The sequence before it will be the first parameter and the 
		*	sequence after it will be the second parameter
		*/
		
		void TreatAsSequence(CompilerContext& context, Project* Proj, CallSequence* sequence)
		{
			int Start = context.OutputString->length();
			std::pair<Sequence*, Project*> Seq;
			
			//We know the current node isn't an operator. Before doing anything else, make sure the next node isn't a two-parameter operator
			if (context.NodeIdx + 1 < context.Nodes->size())
			{
				AST_Node* next = (*context.Nodes)[context.NodeIdx + 1];

				if (next->GetName() == "CallSequence")
				{
					//Check if it has a location (operators are all global)
					CallSequence* SeqNode = (CallSequence*)next;
					if (SeqNode->Location.size() == 0)
					{
						//Check if it's an operator
						Seq = Proj->GetOperator(SeqNode->Name, &Modules);
						if (Seq.first != nullptr)
						{
							//Check if it's a two-parameter operator
							if (Seq.first->ParamIdx2Name.size() == 2)
							{
								//We know it's an operator that will affect this node, so we can just return
								return;
							}

							//Check if it's a 1-parameter operator AND the node after the operator isn't a sequence
							if (context.NodeIdx + 2 == context.Nodes->size() || (*context.Nodes)[context.NodeIdx + 2]->GetName() == "CallSequence")
							{
								//The operator will have no choice but to use this node, we should return
								return;
							}
						}
					}
				}
			}

			//Now we know the next node will not use this sequence. We can safely compile the sequence
			Seq = Proj->GetSeq(sequence, &Modules);
			if (Seq.first == nullptr)
			{
				CDB_BuildError("Sequence \"{0}\" does not exist", sequence->Name);
				return;
			}
			
			//If the sequence's origin is nullptr, it came from a module
			if (Seq.second == nullptr)
				Seq.second = Proj;
			Seq.second->TargetOrganism = Proj->TargetOrganism;
			
			//Get the params
			auto params = sequence->Params.ToParamMap(context, Proj, Seq.first->ParamIdx2Name);
			auto OldParams = context.Params;
			context.Params = &params;

			//Now compile the parameter
			Seq.first->Get(Seq.second, params, context);

			context.Params = OldParams;
			context.OutputRegions->push_back(Region(sequence->Name, Start, context.OutputString->length()));
		}

		void TreatAsOperator(CompilerContext& context, Project* Proj, CallSequence* sequence, std::pair<Sequence*, Project*> Operator)
		{
			//First things first, we need to find out how many parameters the operator has
			if (Operator.first->ParamIdx2Name.size() == 2)
			{
				std::pair<Sequence*, Project*> LPARAM;
				std::pair<Sequence*, Project*> RPARAM;

				
				bool LPARAM_Exists = context.NodeIdx - 1 >= 0 && (*context.Nodes)[context.NodeIdx - 1]->GetName() == "CallSequence";
				bool RPARAM_Exists = context.NodeIdx + 1 < context.Nodes->size() && (*context.Nodes)[context.NodeIdx + 1]->GetName() == "CallSequence";
				
				if (LPARAM_Exists != RPARAM_Exists)
				{
					CDB_BuildError("Operator \"{0}\" expected two parameters but found one", sequence->Name);
					return;
				}
				else if (!LPARAM_Exists && !RPARAM_Exists)
				{
					CDB_BuildError("Operator \"{0}\" expected two parameters but found none", sequence->Name);
					return;
				}

				LPARAM = Proj->GetSeq((CallSequence*)(*context.Nodes)[context.NodeIdx - 1], &Modules);

				if (LPARAM.first == nullptr)
				{
					CDB_BuildError("Sequence passed as left parameter to operator \"{0}\" does not exist", sequence->Name);
					return;
				}
				//If the sequence's origin is nullptr, it came from a module
				if (LPARAM.second == nullptr)
					LPARAM.second = Proj;
				LPARAM.second->TargetOrganism = Proj->TargetOrganism;
				
				RPARAM = Proj->GetSeq((CallSequence*)(*context.Nodes)[context.NodeIdx + 1], &Modules);

				if (RPARAM.first == nullptr)
				{
					CDB_BuildError("Sequence passed as right parameter to operator \"{0}\" does not exist", sequence->Name);
					return;
				}
				//If the sequence's origin is nullptr, it came from a module
				if (RPARAM.second == nullptr)
					RPARAM.second = Proj;
				RPARAM.second->TargetOrganism = Proj->TargetOrganism;
				
				++context.NodeIdx;
				
				//Now we can compile the operator using the parameter
				//Extract the parameter into a params dictionary
				std::map<std::string, Param> Params = {
					{ Operator.first->ParamIdx2Name[0], Param(LPARAM.first, LPARAM.second, LPARAM.first->SeqType) },
					{ Operator.first->ParamIdx2Name[1], Param(RPARAM.first, RPARAM.second, RPARAM.first->SeqType) }
				};

				auto OldParams = context.Params;
				context.Params = &Params;

				Operator.first->Get(Proj, Params, context);

				context.Params = OldParams;
			}
			else if (Operator.first->ParamIdx2Name.size() == 1)
			{
				std::pair<Sequence*, Project*> PARAM;
				
				bool NextNodeExists = context.NodeIdx + 1 < context.Nodes->size();
				bool LastNodeExists = context.NodeIdx - 1 >= 0;
				
				bool NextNode = NextNodeExists && (*context.Nodes)[context.NodeIdx + 1]->GetName() == "CallSequence";
				bool LastNode = LastNodeExists && (*context.Nodes)[context.NodeIdx - 1]->GetName() == "CallSequence";

				//We want to use the next node if it's on the same line and exists
				bool UseNextNode = NextNode && (*context.Nodes)[context.NodeIdx + 1]->pos.Line == sequence->pos.Line;
				
				//We want to use the previous node if it's the only option or the next node is on a different line
				bool UseLastNode = (LastNode && !NextNode) || (LastNode && NextNode && (*context.Nodes)[context.NodeIdx + 1]->pos.Line != sequence->pos.Line);
				
				
				//First, check if the next node is a sequence
				if (UseNextNode)
				{
					CallSequence* SeqNode = (CallSequence*)(*context.Nodes)[context.NodeIdx + 1];
					PARAM = Proj->GetSeq(SeqNode, &Modules);
					
					//Make sure the sequence exists
					if (PARAM.first == nullptr)
					{
						CDB_BuildError("Sequence \"{0}\" passed as parameter to operator \"{1}\" does not exist", SeqNode->Name, sequence->Name);
						return;
					}
					
					++context.NodeIdx;
				}
				//Check if the previous node was a sequence
				else if (UseLastNode)
				{
					CallSequence* SeqNode = (CallSequence*)(*context.Nodes)[context.NodeIdx - 1];
					PARAM = Proj->GetSeq(SeqNode, &Modules);
					
					//Make sure the sequence exists
					if (PARAM.first == nullptr)
					{
						CDB_BuildError("Sequence \"{0}\" passed as parameter to operator \"{1}\" does not exist", SeqNode->Name, sequence->Name);
						return;
					}
				}
				else
				{
					CDB_BuildError("Operator \"{0}\" expected one parameter, found none", sequence->Name);
					return;
				}

				//If the sequence's origin is nullptr, it came from a module
				if (PARAM.second == nullptr)
					PARAM.second = Proj;
				PARAM.second->TargetOrganism = Proj->TargetOrganism;

				
				//Now we can compile the operator using the parameter
				//Extract the parameter into a params dictionary
				std::map<std::string, Param> Params = {
					{ Operator.first->ParamIdx2Name[0], Param(PARAM.first, PARAM.second, PARAM.first->SeqType) }
				};

				auto OldParams = context.Params;
				context.Params = &Params;				
				
				Operator.first->Get(Proj, Params, context);
				
				context.Params = OldParams;
			}
			else
			{
				//This operator has the wrong number of parameters
				CDB_BuildError("Operator \"{0}\" can only be called on one or two parameters, requested {1}", sequence->Name, Operator.first->ParamIdx2Name.size());
			}
		}
		
		void CallSequence::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			int Start = context.OutputString->length();
			if (this->Location.size() != 0 || this->Params.Params.size() != 0)
			{
				TreatAsSequence(context, Project, this);
				context.OutputRegions->push_back(Region(this->Name, Start, context.OutputString->length()));
				return;
			}
			
			//Check if it might be an operator
			auto Seq = Project->GetOperator(this->Name, &Modules);
			if (Seq.first != nullptr)
				TreatAsOperator(context, Project, this, Seq);
			else
				TreatAsSequence(context, Project, this);
		}
		
		void CallSequence::Save(std::ofstream& OutputFile)
		{
			SaveString(this->Name, OutputFile);
			SaveStringVector(this->Location, OutputFile);
			this->Params.Save(OutputFile);
		}
		
		void CallSequence::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->Name, InputFile);
			LoadStringVectorFromFile(this->Location, InputFile);
			this->Params.Load(InputFile, Proj);
		}
		
		void CallOperation::Compile(Compiler::CompilerContext& context, Parser::Project* Proj)
		{
			auto Op = Proj->GetSeq(this, &Modules);
			if (Op.first == nullptr)    //Make sure sequence exists
			{
				CDB_BuildError("Operation \"{0}\" does not exist", this->Name);
				return;
			}
			
			//If Op's project is nullptr, it came from a module
			if (Op.second == nullptr)
				Op.second = Proj;
			Op.second->TargetOrganism = Proj->TargetOrganism;
			
			
			auto params = this->Params.ToParamMap(context, Proj, Op.first->ParamIdx2Name);
			
			//Create a new compiler context for innercode
			std::vector<Region> OutputRegions;
			std::vector<Region> OpenRegions;
			std::string OutputCode;
			
			CompilerContext InnerCodeContext = { &this->InnerNodes, &OutputRegions, &OutputCode, context.Encoding, context.Params, 0, &OpenRegions };
			for (InnerCodeContext; InnerCodeContext.NodeIdx < InnerCodeContext.Nodes->size(); ++InnerCodeContext.NodeIdx)
			{
				(*InnerCodeContext.Nodes)[InnerCodeContext.NodeIdx]->Compile(InnerCodeContext, Proj);
			}
			
			InnerCode innerCode = InnerCode({ OutputRegions, OutputCode });
			params["InnerCode"] = Param(&innerCode, Proj);

			Op.first->Get(Op.second, params, context);
		}
		
		void CallOperation::Save(std::ofstream& OutputFile)
		{
			SaveString(this->Name, OutputFile);
			this->Params.Save(OutputFile);
			
			SaveSize(this->InnerNodes.size(), OutputFile);
			for (AST_Node* node : this->InnerNodes)
			{
				SaveNode(node, OutputFile);
			}
			
			SaveStringVector(this->Location, OutputFile);
		}
		
		void CallOperation::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->Name, InputFile);
			this->Params.Load(InputFile, Proj);
			
			size_t NumNodes = LoadSizeFromFile(InputFile);
			this->InnerNodes.reserve(NumNodes);
			
			for (int i = 0; i < NumNodes; ++i)
			{
				this->InnerNodes.push_back(LoadNode(InputFile, Proj));
			}
			
			LoadStringVectorFromFile(this->Location, InputFile);
		}
		
		void SetTarget::AddToProject(Parser::Project* Proj)
		{
			Proj->TargetOrganism = this->Target;
		}
		
		void BeginRegion::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			context.OpenRegions->push_back(Region(this->RegionName, context.OutputString->size(), 0));
		}
		
		void BeginRegion::Save(std::ofstream& OutputFile)
		{
			SaveString(this->RegionName, OutputFile);
		}
		
		void BeginRegion::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->RegionName, InputFile);
		}
		
		void EndRegion::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//If the region isn't ended by name, just end the last region
			if (this->RegionName == "")
			{
				(*context.OpenRegions)[context.OpenRegions->size() - 1].End = context.OutputString->size();
				context.OutputRegions->push_back(std::move((*context.OpenRegions)[context.OpenRegions->size() - 1]));
				context.OpenRegions->pop_back();
				return;
			}
			
			//Find the region with the name given
			for (int i = context.OpenRegions->size() - 1; i >= 0; --i)
			{
				if ((*context.OpenRegions)[i].Name == this->RegionName)
				{
					(*context.OpenRegions)[i].End = context.OutputString->size();
					context.OutputRegions->push_back(std::move((*context.OpenRegions)[i]));
					context.OpenRegions->erase(context.OpenRegions->begin() + i);
					return;
				}
			}
			//We didn't find the region, throw an error
			CDB_BuildError("Region \"{0}\" ended explicitly by #EndRegion does not exist", this->RegionName);
		}
		
		void EndRegion::Save(std::ofstream& OutputFile)
		{
			SaveString(this->RegionName, OutputFile);
		}
		
		void EndRegion::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->RegionName, InputFile);
		}
		
		void SetAttr::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			if (this->AttrName == "")
			{
				CDB_BuildError("Attribute name is empty");
				return;
			}
			
			Project->Attributes[this->AttrName] = this->AttrValue;
		}
		
		void SetAttr::Save(std::ofstream& OutputFile)
		{
			SaveString(this->AttrName, OutputFile);
			SaveString(this->AttrValue, OutputFile);
		}
		
		void SetAttr::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->AttrName, InputFile);
			LoadStringFromFile(this->AttrValue, InputFile);
		}
		
		void SetVar::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			if (this->VarName == "")
			{
				CDB_BuildError("Variable name is empty");
				return;
			}

			//Check whether it's a string or number variable
			if (this->m_VariableType == VariableType::num)
			{
				if (this->VarValue == "")
				{
					Project->NumVars[this->VarName] = 0;
					return;
				}

				//There's probably a better way to do this
				try
				{
					Project->NumVars[this->VarName] = std::stod(this->VarValue);
				}
				catch (std::invalid_argument e)
				{
					CDB_BuildError("Cannot set numerical variable \"{0}\" to value {1}", this->VarName, this->VarValue);
				}
				return;
			}

			//It's a string variable, just store the value
			Project->StrVars[this->VarName] = this->VarValue;
		}
		
		void SetVar::Save(std::ofstream& OutputFile)
		{
			OutputFile.write((char*)&this->m_VariableType, sizeof(VariableType));
			SaveString(this->VarName, OutputFile);
			SaveString(this->VarValue, OutputFile);
		}

		void SetVar::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			InputFile.read((char*)&this->m_VariableType, sizeof(VariableType));
			LoadStringFromFile(this->VarName, InputFile);
			LoadStringFromFile(this->VarValue, InputFile);
		}

		void IncVar::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//Make sure the variable exists
			if (!Project->NumVars.contains(this->VarName))
			{
				CDB_BuildError("Attempt to increment variable \"{0}\" which does not exist", this->VarName);
				return;
			}

			++Project->NumVars[this->VarName];
		}

		void IncVar::Save(std::ofstream& OutputFile)
		{
			SaveString(this->VarName, OutputFile);
		}

		void IncVar::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->VarName, InputFile);
		}

		void DecVar::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//Make sure the variable exists
			if (!Project->NumVars.contains(this->VarName))
			{
				CDB_BuildError("Attempt to increment variable \"{0}\" which does not exist", this->VarName);
				return;
			}

			--Project->NumVars[this->VarName];
		}

		void DecVar::Save(std::ofstream& OutputFile)
		{
			SaveString(this->VarName, OutputFile);
		}

		void DecVar::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->VarName, InputFile);
		}

		void Prepro_If::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//Make sure the condition exists
			if (!PreproConditions.contains(this->Condition))
			{
				CDB_BuildError("The preprocessor condition \"{0}\" does not exist", this->Condition);
			}
			bool output = PreproConditions[this->Condition](Project, &this->Params);

			//Create the new context
			CompilerContext BodyContext = context;
			BodyContext.NodeIdx = 0;

			//Compile the body
			if (output)
			{
				BodyContext.Nodes = &this->Body;
			}
			//Otherwise, check if there's an else statement and compile that
			else
			{
				if ((*context.Nodes)[context.NodeIdx + 1]->GetName() == "Prepro_Else")
				{
					Prepro_Else* ElseNode = (Prepro_Else*)(*context.Nodes)[context.NodeIdx + 1];

					BodyContext.Nodes = &ElseNode->Body;
				}
			}

			//Compile the nodes
			for (BodyContext.NodeIdx; BodyContext.NodeIdx < BodyContext.Nodes->size(); ++BodyContext.NodeIdx)
			{
				(*BodyContext.Nodes)[BodyContext.NodeIdx]->Compile(BodyContext, Project);
			}
		}

		void Prepro_If::Save(std::ofstream& OutputFile)
		{
			SaveString(this->Condition, OutputFile);
			SaveSize(this->Params.size(), OutputFile);
			for (Token* t : this->Params)
				t->Save(OutputFile);

			SaveSize(this->Body.size(), OutputFile);
			for (AST_Node* node : this->Body)
				SaveNode(node, OutputFile);
		}

		void Prepro_If::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->Condition, InputFile);
			size_t size = LoadSizeFromFile(InputFile);
			this->Params.reserve(size);
			for (int i = 0; i < size; ++i)
				this->Params.push_back(Token::Load(InputFile));

			size = -1;
			size = LoadSizeFromFile(InputFile);
			this->Body.reserve(size);
			for (int i = 0; i < size; ++i)
				this->Body.push_back(LoadNode(InputFile, Proj));
		}

		void Prepro_Else::Save(std::ofstream& OutputFile)
		{
			SaveSize(this->Body.size(), OutputFile);
			for (AST_Node* node : this->Body)
				SaveNode(node, OutputFile);
		}
		

		void Prepro_Else::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			size_t size = LoadSizeFromFile(InputFile);
			this->Body.reserve(size);
			for (int i = 0; i < size; ++i)
				this->Body.push_back(LoadNode(InputFile, Proj));
		}

#define RemoveChar(c) c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'
		void RemoveSpacesFromDNA(std::string& DNA)
		{
			size_t InsertIdx = 0;
			for (size_t CurrentIdx = 0; CurrentIdx < DNA.size(); ++CurrentIdx)
			{
				if (RemoveChar(DNA[CurrentIdx]))
				{
					//Just shift the current index by one but keep the insertion index the same
					continue;
				}
				//Insert the character
				DNA[InsertIdx] = DNA[CurrentIdx];
				++InsertIdx;
			}
		}
		
		DNALiteral::DNALiteral(std::string&& Literal)
		{
			this->Literal = std::move(Literal);
			RemoveSpacesFromDNA(this->Literal);
		}
		
		void DNALiteral::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			*context.OutputString += this->Literal;
		}

		void DNALiteral::Save(std::ofstream& OutputFile)
		{
			//Just in case
			RemoveSpacesFromDNA(this->Literal);
			SaveString(this->Literal, OutputFile);
		}

		void DNALiteral::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
			LoadStringFromFile(this->Literal, InputFile);
		}
		
		AccessNamespace::AccessNamespace(std::vector<Token*>&& namespaces)
		{
			this->Namespaces.reserve(namespaces.size());
			for (Token* tok : namespaces)
			{
				this->Namespaces.push_back(std::move(tok->Value));
			}
		}
		
		void Namespace::AddToProject(Parser::Project* Proj)
		{
			//Create a new project
			Parser::Project* NewProj = new Parser::Project();
			
			//Copy the old project's settings to the new one
			NewProj->TargetOrganism = Proj->TargetOrganism;

			//Add the nodes to this subproject
			for (AST_Node* node : this->Body)
			{
				//The node has an AddToProject function
				if (node->CanAddToProject())
				{
					ProjectNode* pnode = (ProjectNode*)node;
					pnode->AddToProject(NewProj);
				}
			}
			
			//Add the subproject to the project
			Proj->Namespaces[this->Name] = NewProj;
		}
	}
}