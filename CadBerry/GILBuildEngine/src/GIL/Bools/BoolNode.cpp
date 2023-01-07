#include <gilpch.h>
#include "BoolNode.h"
#include "GIL/Parser/Project.h"
#include "GIL/Errors.h"
#include "GIL/GILException.h"
#include "GIL/Parser/AST_Node.h"
#include "GIL/MacroUtils.h"


namespace GIL
{
	namespace Parser
	{
		using namespace Compiler;
		N_UseBool::N_UseBool(BoolNode* boolref, std::vector<AST_Node*>&& nodes)
		{
			this->boolref = boolref;
			this->nodes = nodes;
			
		}
		
		std::vector<N_UseBool*> MakeIfStatements(BoolNode* boolref, std::vector<AST_Node*>& nodes, Project* project)
		{
			//Because all if statements are being executed in parallel, we can't have nested if statements. This function lifts nested if 
			//statements out of the parent if statement to make them valid

			//We also need to keep a record of every bool graph in the project file so that the opimizer can take these into account
			
			std::vector<N_UseBool*> output = { new N_UseBool(boolref, {}) };
			
			for (int i = 0; i < nodes.size(); ++i)
			{
				if (nodes[i]->GetName() == "N_UseBool")
				{
					//pull node out of substatement
					N_UseBool* node = (N_UseBool*)nodes[i];
					BoolNode* cond = node->boolref;
					node->boolref = new N_And(boolref, cond);
					
					project->GraphHeads.push_back(node->boolref);
					output.push_back(node);
				}
				else if (nodes[i]->GetName() == "N_SetBool")
				{
					//Pull node out of statement
					N_SetBool* node = (N_SetBool*)nodes[i];
					BoolNode* cond = node->Value;
					node->Value = new N_And(boolref, cond);
					
					project->GraphHeads.push_back(node->Value);
					project->AddedBoolOps.push_back(node);
				}
				else
				{				
					output[0]->nodes.push_back(nodes[i]);
				}
			}
			project->GraphHeads.push_back(boolref);
			return output;
		}
#define func() std::pair<std::string, std::vector<Region>>()
#define Require(cond, line, msg) if (!(cond)) { CDB_BuildError(msg, line); throw GILException(); }
		
		std::pair<std::string, std::vector<Region>> CompileChainedAND(GILBool* b, Compiler::CompilerContext& context, Parser::Project* Proj, 
			N_And* node, InlineSequence* useseq, ParserPosition& pos, bool LastCall = false)
		{
			std::vector<Region> Output;
			std::vector<Region> OpenRegions;
			std::string Code;
			CompilerContext stub = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			//Compile left side
			std::map<std::string, Param> Params;
			InlineSequence empty("", {});
			if (node->left->GetType() == BoolNodeType::AND)
			{
				split CompileChainedAND(b, stub, Proj, (N_And*)node->left, useseq, pos) to(Code, Output);
			}
			else if (node->left->GetType() == BoolNodeType::PLACEHOLDER)
			{
				//Make the placeholder's name
				N_Placeholder* placeholder_node = (N_Placeholder*)node->left;
				std::string name = "";
				for (int i = 0; i < placeholder_node->Location.Namespaces.size(); ++i)
				{
					name += placeholder_node->Location.Namespaces[i] + "@";
				}
				name += placeholder_node->Name;

				//Make sure the placeholder exists
				if (!Proj->LocalBools.contains(name))
				{
					//TODO: make this an error in Errors.h
					CDB_BuildError("Placeholder \"{0}\" on line {1} does not exist", name, pos.Line);
					throw GILException();
				}

				DataSequence nameseq = DataSequence(&name);

				//Get the placeholder's node
				Params = {
					{ "stub", Param(&empty, Proj) },
					{ "val", Param(&nameseq, Proj) },
					{ "output", Param(&empty, Proj) }
				};

				b->impl->ChainedAnd->Get(Proj, Params, stub);
			}
			InlineSequence inl = InlineSequence(std::move(Code), std::move(Output));
			
			Code = "";
			Output = {};
			stub = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			//Compile the right node
			if (node->right->GetType() == BoolNodeType::AND)
			{
				split CompileChainedAND(b, stub, Proj, (N_And*)node->right, useseq, pos, LastCall) to(Code, Output);
			}
			else if (node->right->GetType() == BoolNodeType::PLACEHOLDER)
			{
				//Make the placeholder's name
				N_Placeholder* placeholder_node = (N_Placeholder*)node->left;
				std::string name = "";
				for (int i = 0; i < placeholder_node->Location.Namespaces.size(); ++i)
				{
					name += placeholder_node->Location.Namespaces[i] + "@";
				}
				name += placeholder_node->Name;

				DataSequence nameseq = DataSequence(&name);

				//Get the placeholder's node
				if (LastCall)
				{
					Params = {
						{ "stub", Param(&inl, Proj) },
						{ "val", Param(&nameseq, Proj) },
						{ "output", Param(useseq, Proj) }
					};
				}
				else
				{
					Params = {
						{ "stub", Param(&inl, Proj) },
						{ "val", Param(&nameseq, Proj) },
						{ "output", Param(&empty, Proj) }
					};
				}

				b->impl->ChainedAnd->Get(Proj, Params, stub);
			}
			return { Code, Output };
		}

		std::pair<std::string, std::vector<Region>> CompileChainedOR(GILBool* b, Compiler::CompilerContext& context, Parser::Project* Proj,
			N_Or* node, InlineSequence* useseq, ParserPosition& pos, bool LastCall = false)
		{
			std::vector<Region> Output;
			std::vector<Region> OpenRegions;
			std::string Code;
			CompilerContext stub = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			//Compile left side
			std::map<std::string, Param> Params;
			InlineSequence empty("", {});
			if (node->left->GetType() == BoolNodeType::OR)
			{
				split CompileChainedOR(b, stub, Proj, (N_Or*)node->left, useseq, pos) to(Code, Output);
			}
			else if (node->left->GetType() == BoolNodeType::PLACEHOLDER)
			{
				//Make the placeholder's name
				N_Placeholder* placeholder_node = (N_Placeholder*)node->left;
				std::string name = "";
				for (int i = 0; i < placeholder_node->Location.Namespaces.size(); ++i)
				{
					name += placeholder_node->Location.Namespaces[i] + "@";
				}
				name += placeholder_node->Name;

				DataSequence nameseq = DataSequence(&name);

				//Get the placeholder's node
				Params = {
					{ "stub", Param(&empty, Proj) },
					{ "val", Param(&nameseq, Proj) },
					{ "output", Param(&empty, Proj) }
				};

				b->impl->ChainedOr->Get(Proj, Params, stub);
			}
			InlineSequence inl = InlineSequence(std::move(Code), std::move(Output));

			Code = "";
			Output = {};
			stub = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			//Compile the right node
			if (node->right->GetType() == BoolNodeType::OR)
			{
				split CompileChainedOR(b, stub, Proj, (N_Or*)node->right, useseq, pos, LastCall) to(Code, Output);
			}
			else if (node->right->GetType() == BoolNodeType::PLACEHOLDER)
			{
				//Make the placeholder's name
				N_Placeholder* placeholder_node = (N_Placeholder*)node->left;
				std::string name = "";
				for (int i = 0; i < placeholder_node->Location.Namespaces.size(); ++i)
				{
					name += placeholder_node->Location.Namespaces[i] + "@";
				}
				name += placeholder_node->Name;

				DataSequence nameseq = DataSequence(&name);

				//Get the placeholder's node
				if (LastCall)
				{
					Params = {
						{ "stub", Param(&inl, Proj) },
						{ "val", Param(&nameseq, Proj) },
						{ "output", Param(useseq, Proj) }
					};
				}
				else
				{
					Params = {
						{ "stub", Param(&inl, Proj) },
						{ "val", Param(&nameseq, Proj) },
						{ "output", Param(&empty, Proj) }
					};
				}

				b->impl->ChainedOr->Get(Proj, Params, stub);
			}
			return { Code, Output };
		}
		
		void CompileCondition(GILBool* b, Compiler::CompilerContext& context, Parser::Project* Proj, BoolNode* cond, InlineSequence* useseq,
			ParserPosition& pos)
		{
			bool ChainedAND = b->impl->ChainedAndAvailable;
			bool ChainedOR = b->impl->ChainedOrAvailable;

			switch (cond->GetType())
			{
			case BoolNodeType::PLACEHOLDER:
			{
				//Placeholder nodes represent another bool being used. Get that bool's implementation and use it on this bool
				N_Placeholder* node = (N_Placeholder*)cond;
				std::string name = "";
				for (int i = 0; i < node->Location.Namespaces.size(); ++i)
				{
					name += node->Location.Namespaces[i] + "@";
				}
				name += node->Name;

				if (!Proj->LocalBools.contains(name))
				{
					CDB_BuildError(ERROR_047, pos.Line, node->Name, name);
					throw GILException();
				}

				//Get the bool from the project
				GILBool* usedbool = Proj->LocalBools[name];

				//Make sure the bool has an implementation
				if (usedbool->impl == nullptr)
				{
					CDB_BuildError(ERROR_048, node->Name, name);
					throw GILException();
				}
				
				//Call the bool's use sequence on this bool
				std::map<std::string, Param> Params = { {"inner", Param(useseq, Proj)} };
				usedbool->impl->Use->Get(Proj, Params, context);
				break;
			}
			case BoolNodeType::AND:
			{
				N_And* node = (N_And*)cond;
				
				//If the bool supports chained ANDs, we need to start with the leftmost node and work our way to the right
				if (ChainedAND)
				{
					std::string addedcode;
					std::vector<Region> addedregions;
					split CompileChainedAND(b, context, Proj, node, useseq, pos, true) to(addedcode, addedregions);
					*context.OutputString += addedcode;
					AddVectors(*context.OutputRegions, addedregions);
				}
				else
				{
					//If the bool doesn't support chained ANDs, this is the only node that we need to compile
					Require(node->left->GetType() == BoolNodeType::PLACEHOLDER, pos.Line, ERROR_049);
					Require(node->right->GetType() == BoolNodeType::PLACEHOLDER, pos.Line, ERROR_049);
					
					//Get the left and right nodes
					N_Placeholder* left = (N_Placeholder*)node->left;
					N_Placeholder* right = (N_Placeholder*)node->right;
					std::string lname = "";
					std::string rname = "";
					for (int i = 0; i < left->Location.Namespaces.size(); ++i)
					{
						lname += left->Location.Namespaces[i] + "@";
					}
					for (int i = 0; i < right->Location.Namespaces.size(); ++i)
					{
						rname += right->Location.Namespaces[i] + "@";
					}
					lname += left->Name;
					rname += right->Name;

					//Make sure the bools exist
					if (!Proj->LocalBools.contains(lname))
					{
						CDB_BuildError(ERROR_047, pos.Line, left->Name, lname);
						throw GILException();
					}
					if (!Proj->LocalBools.contains(rname))
					{
						CDB_BuildError(ERROR_047, pos.Line, right->Name, rname);
						throw GILException();
					}
					
					//Make data sequences for the left and right nodes
					DataSequence leftseq = DataSequence(&lname);
					DataSequence rightseq = DataSequence(&rname);
					
					//Construct the params
					std::map<std::string, Param> Params = { 
						{"left", Param(&leftseq, Proj)},
						{"right", Param(&rightseq, Proj)},
						{"output", Param(useseq, Proj)}
					};
					
					//Call the bool's AND sequence
					b->impl->And->Get(Proj, Params, context);
				}
				break;
			}
			case BoolNodeType::OR:
			{
				N_Or* node = (N_Or*)cond;

				//If the bool supports chained ANDs, we need to start with the leftmost node and work our way to the right
				if (ChainedOR)
				{
					std::string addedcode;
					std::vector<Region> addedregions;
					split CompileChainedOR(b, context, Proj, node, useseq, pos, true) to(addedcode, addedregions);
					*context.OutputString += addedcode;
					AddVectors(*context.OutputRegions, addedregions);
				}
				else
				{
					//If the bool doesn't support chained ANDs, this is the only node that we need to compile
					Require(node->left->GetType() == BoolNodeType::PLACEHOLDER, pos.Line, ERROR_049);
					Require(node->right->GetType() == BoolNodeType::PLACEHOLDER, pos.Line, ERROR_049);

					//Get the left and right nodes
					N_Placeholder* left = (N_Placeholder*)node->left;
					N_Placeholder* right = (N_Placeholder*)node->right;
					std::string lname = "";
					std::string rname = "";
					for (int i = 0; i < left->Location.Namespaces.size(); ++i)
					{
						lname += left->Location.Namespaces[i] + "@";
					}
					for (int i = 0; i < right->Location.Namespaces.size(); ++i)
					{
						rname += right->Location.Namespaces[i] + "@";
					}
					lname += left->Name;
					rname += right->Name;

					//Make sure the bools exist
					if (!Proj->LocalBools.contains(lname))
					{
						CDB_BuildError(ERROR_047, pos.Line, left->Name, lname);
						throw GILException();
					}
					if (!Proj->LocalBools.contains(rname))
					{
						CDB_BuildError(ERROR_047, pos.Line, right->Name, rname);
						throw GILException();
					}

					//Make data sequences for the left and right nodes
					DataSequence leftseq = DataSequence(&lname);
					DataSequence rightseq = DataSequence(&rname);

					//Construct the params
					std::map<std::string, Param> Params = {
						{"left", Param(&leftseq, Proj)},
						{"right", Param(&rightseq, Proj)},
						{"output", Param(useseq, Proj)}
					};

					//Call the bool's OR sequence
					b->impl->Or->Get(Proj, Params, context);
				}
				break;
			}
			case BoolNodeType::NOT:
			{
				N_Not* node = (N_Not*)cond;
				Require(node->node->GetType() == BoolNodeType::PLACEHOLDER, pos.Line, ERROR_049);
				N_Placeholder* input = (N_Placeholder*)node->node;
				std::string iname = "";
				for (int i = 0; i < input->Location.Namespaces.size(); ++i)
				{
					iname += input->Location.Namespaces[i] + "@";
				}
				iname += input->Name;
				
				//Make sure the bool exists
				if (!Proj->LocalBools.contains(iname))
				{
					CDB_BuildError(ERROR_047, pos.Line, input->Name, iname);
					throw GILException();
				}
				
				//Make data sequences for the input node
				DataSequence inputseq = DataSequence(&iname);
				
				//Construct the params
				std::map<std::string, Param> Params = {
					{"input", Param(&inputseq, Proj)},
					{"output", Param(useseq, Proj)}
				};
				
				//Call the bool's NOT sequence
				b->impl->Not->Get(Proj, Params, context);

				break;
			}
			}
		}
#undef Require
		
		void N_SetBool::Compile(Compiler::CompilerContext& context, Parser::Project* Proj)
		{
			//Get the bool
			std::vector<std::string>& loc = this->Location.Namespaces;
			
			//Convert the location to a full name
			std::string name = "";
			for (int i = 0; i < loc.size(); ++i)
			{
				name += loc[i] + "@";
			}
			name += this->Bool;

			if (!Proj->LocalBools.contains(name))
			{
				CDB_BuildError(ERROR_047, this->pos.Line, this->Bool, name);
				throw GILException();
			}
			
			//Get the bool from the project
			GILBool* b = Proj->LocalBools[name];
			
			//Make sure the bool has an implementation
			if (b->impl == nullptr)
			{
				CDB_BuildError(ERROR_048, this->Bool, name);
				throw GILException();
			}
			
			//Get the bool's set sequence
			Sequence* SetBool = b->impl->Set;
			
			//Compile the set sequence and convert it into an inline sequence to pass to the logic gates
			std::vector<Region> Output;
			std::vector<Region> OpenRegions;
			std::string Code;
			CompilerContext tmpcontext = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			
			DataSequence nameseq = DataSequence(&name);
			std::map<std::string, Param> Params = { { "name", Param(&nameseq, Proj, &Type::data)}};
			
			SetBool->Get(Proj, Params, tmpcontext);
			
			InlineSequence seq = InlineSequence(std::move(Code), std::move(Output));

			
			//Finally, compile the bool's condition. This will behave differently if the implementation supports chained ANDs and ORs
			CompileCondition(b, context, Proj, this->Value, &seq, this->pos);
		}


		void N_SetBoolTrue::Compile(Compiler::CompilerContext& context, Parser::Project* Proj)
		{
			std::vector<std::string>& loc = this->Location.Namespaces;

			//Convert the location to a full name
			std::string name = "";
			for (int i = 0; i < loc.size(); ++i)
			{
				name += loc[i] + "@";
			}
			name += this->Name;

			if (!Proj->LocalBools.contains(name))
			{
				CDB_BuildError(ERROR_047, this->pos.Line, this->Name, name);
				throw GILException();
			}

			//Get the bool from the project
			GILBool* b = Proj->LocalBools[name];

			//Make sure the bool has an implementation
			if (b->impl == nullptr)
			{
				CDB_BuildError(ERROR_048, this->Name, name);
				throw GILException();
			}

			//Construct params
			DataSequence seq = DataSequence(&name);
			std::map<std::string, Param> Params = { {"name", Param(&seq, Proj)} };
			b->impl->Set->Get(Proj, Params, context);
		}


		void N_UseBool::Compile(Compiler::CompilerContext& context, Parser::Project* Proj)
		{
			//First, compile the inner nodes
			std::vector<Region> Output;
			std::vector<Region> OpenRegions;
			std::string Code;
			CompilerContext InnerContext = { &this->nodes, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };

			for (AST_Node* node : this->nodes)
			{
				node->Compile(InnerContext, Proj);
			}

			//Convert to inline sequence
			InlineSequence InnerSeq = InlineSequence(std::move(Code), std::move(Output));

			//Construct params
			std::map<std::string, Param> Params = { {"inner", Param(&InnerSeq, Proj)} };

			//Find placeholder node (all graphs have to lead to a placeholder node if you travel back far enough)
			BoolNode* bn = this->boolref;
			for (int itter = 0; itter < 9999999999 && bn->GetType() != BoolNodeType::PLACEHOLDER; ++itter)
			{
				switch (bn->GetType())
				{
				case BoolNodeType::AND:
					bn = reinterpret_cast<N_And*>(bn)->left;
					break;
				case BoolNodeType::OR:
					bn = reinterpret_cast<N_Or*>(bn)->left;
					break;
				case BoolNodeType::NOT:
					bn = reinterpret_cast<N_Not*>(bn)->node;
				default:
					break;
				}
			}
			N_Placeholder* placeholder = (N_Placeholder*)bn;
			//Get the bool (for the implementation)
			std::string name = "";
			for (std::string& ns : placeholder->Location.Namespaces)
			{
				name += ns + "@";
			}
			name += placeholder->Name;

			if (!Proj->LocalBools.contains(name))
			{
				CDB_BuildError(ERROR_047, this->pos.Line, placeholder->Name, name);
				throw GILException();
			}

			//Get the bool from the project
			GILBool* b = Proj->LocalBools[name];

			//Make sure the bool has an implementation
			if (b->impl == nullptr)
			{
				CDB_BuildError(ERROR_048, placeholder->Name, name);
				throw GILException();
			}

			//Compile the bool's condition
			CompileCondition(b, context, Proj, this->boolref, &InnerSeq, this->pos);
		}
		
		void N_BoolInput::Compile(Compiler::CompilerContext& context, Parser::Project* Project)
		{
			//Get the bool from the project
			GILBool* b = Project->LocalBools[this->BoolID];

			//Make sure the bool has an implementation
			if (b->impl == nullptr)
			{
				CDB_BuildError(ERROR_048, "temporary bool", this->BoolID);
				throw GILException();
			}

			//Get the bool's set implementation
			std::vector<Region> Output;
			std::vector<Region> OpenRegions;
			std::string Code;
			CompilerContext tmpcontext = { nullptr, &Output, &Code, context.Encoding, nullptr, 0, &OpenRegions, context.Distribution };
			
			DataSequence seq = DataSequence(&this->BoolID);
			std::map<std::string, Param> Params = { {"name", Param(&seq, Project)} };
			b->impl->Set->Get(Project, Params, tmpcontext);
			
			//Convert to inline sequence
			InnerCode InnerSeq = InnerCode({ std::move(Output), std::move(Code) });
			
			//Call the input sequence on the bool
			Params = { {"InnerCode", Param(&InnerSeq, Project)} };
			this->Seq->Get(this->Origin, Params, context);
		}
	}
}