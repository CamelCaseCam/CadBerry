#include <gilpch.h>
#include "BoolCompiler.h"
#include "BuiltinAllocators.h"
#include "GIL/Compiler/CompilerFunctions.h"
#include "GIL/GILException.h"

namespace GIL
{
	namespace Compiler
	{
		using namespace Parser;
		
		BoolAllocator* NaiveAllocatorInstance = new NaiveAllocator();
		std::unordered_map<std::string, BoolAllocator*> boolAllocators = {
			{ "NaiveAllocator",  NaiveAllocatorInstance},
		};
		BoolAllocator* CurrentAllocator = NaiveAllocatorInstance;

		void CopyImplIfNotDefined(Project* Proj, BoolImplementation* impl)
		{
			for (BoolImplementation* ExistingImpl : Proj->BoolImplementations)
			{
				if (ExistingImpl->name == impl->name)
					return;
			}
			Proj->BoolImplementations.push_back(impl);
		}
		
		using namespace Parser;
		void ChangeGraphNames(BoolNode* node, std::map<std::string, std::string> NameChanges)
		{
			switch (node->GetType())
			{
			case BoolNodeType::AND:
			{
				N_And* n = (N_And*)node;
				ChangeGraphNames(n->left, NameChanges);
				ChangeGraphNames(n->right, NameChanges);
				break;
			}
			case BoolNodeType::OR:
			{
				N_Or* n = (N_Or*)node;
				ChangeGraphNames(n->left, NameChanges);
				ChangeGraphNames(n->right, NameChanges);
				break;
			}
			case BoolNodeType::NOT:
			{
				N_Not* n = (N_Not*)node;
				ChangeGraphNames(n->node, NameChanges);
				break;
			}
			case BoolNodeType::CAST:
			{
				N_Cast* n = (N_Cast*)node;
				ChangeGraphNames(n->node, NameChanges);
				break;
			}
			case BoolNodeType::PLACEHOLDER:
			{
				N_Placeholder* placeholdernode = (N_Placeholder*)node;
				if (NameChanges.contains(placeholdernode->Name))
					placeholdernode->Name = NameChanges[placeholdernode->Name];
				break;
			}
			}
		}

		//This *only* works because any out-of-sequence nodes will be placeholders, which need to be copied anyways
		BoolNode* CopyGraph(BoolNode* node)
		{
			switch (node->GetType())
			{
			case BoolNodeType::AND:
			{
				N_And* n = (N_And*)node;
				//Copy the node
				n = new N_And(*n);
				n->left = CopyGraph(n->left);
				n->right = CopyGraph(n->right);
				return n;
			}
			case BoolNodeType::OR:
			{
				N_Or* n = (N_Or*)node;
				//Copy the node
				n = new N_Or(*n);
				n->left = CopyGraph(n->left);
				n->right = CopyGraph(n->right);
				return n;
			}
			case BoolNodeType::NOT:
			{
				N_Not* n = (N_Not*)node;
				//Copy the node
				n = new N_Not(*n);
				n->node = CopyGraph(n->node);
				return n;
			}
			case BoolNodeType::CAST:
			{
				N_Cast* n = (N_Cast*)node;
				//Copy the node
				n = new N_Cast(*n);
				n->node = CopyGraph(n->node);
				return n;
			}
			case BoolNodeType::PLACEHOLDER:
			{
				N_Placeholder* n = (N_Placeholder*)node;
				n = new N_Placeholder(*n);
				return n;
			}
			}
			return nullptr;
		}

		//void CopyBoolSequenceData(Project* Proj)
		//{
		//	//Find sequence and operation calls
		//	for (AST_Node* node : Proj->Main)
		//	{
		//		if (node->GetName() == "CallSequence")
		//		{
		//			CallSequence* seq = (CallSequence*)node;
		//			
		//			//Get the sequence
		//			auto sequence = Proj->GetSeq(seq, &Modules);
		//			if (sequence.first == nullptr)
		//			{
		//				//This may have been an operator, we should assume the sequence is an operator and continue
		//				continue;
		//			}

		//			std::string runstring = "r" + std::to_string(sequence.first->Callnum);
		//			
		//			auto& bools = sequence.first->GetLocalBools();
		//			auto& GraphHeads = sequence.first->GetGraphHeads();
		//			auto& AddedOps = sequence.first->GetAddedBoolOps();

		//			std::map<std::string, std::string> NameChanges;
		//			//Copy local bools
		//			for (auto& b : bools)
		//			{
		//				//Set new bool name (we'll rewrite the copied graph to point to these nodes)
		//				Proj->LocalBools[seq->Name + "@" + runstring + "@" + b.first] = b.second;
		//				NameChanges[b.first] = seq->Name + "@" + runstring + "@" + b.first;
		//			}

		//			//Copy and edit graph heads
		//			Proj->GraphHeads.reserve(Proj->GraphHeads.size() + GraphHeads.size());
		//			for (auto gh : GraphHeads)
		//			{
		//				//We can't edit graph heads yet since they need to be edited per sequence call
		//				Proj->GraphHeads.push_back(gh);
		//			}

		//			//Copy added ops
		//			Proj->AddedBoolOps.reserve(Proj->AddedBoolOps.size() + AddedOps.size());
		//			for (auto op : AddedOps)
		//			{
		//				Proj->AddedBoolOps.push_back(op);
		//			}
		//			++sequence.first->Callnum;
		//		}
		//	}
		//}
		
		/*
		* This function pulls sequence bool nodes out of the sequence and into the project. It follows the following steps:
		* 1. Get the sequence object
		* 2. Copy the local bools into the project with their new call-specific names
		* 3. Copy the graph heads into the project (edit them to have call-specific names)
		* 4. Get the bool nodes and edit them to point to new graph heads (copy to project at this time)
		* 5. Repeat, but on each naked sequence call within the sequence (not wrapped in if statement)
		* 
		* We don't need to worry about the sequences within if statements, since they'll be inlined by the InlineAllSequences method. We also
		* don't have to worry about iterator invalidation, since the calling sequence uses a index-based for loop. 
		*/
		void InlineSequence(CallSequence* seq, Project* Proj)
		{
			//1. Get the sequence object
			auto sequence = Proj->GetSeq(seq, &Modules);
			if (sequence.first == nullptr)
			{
				//This may have been an operator, we should assume the sequence is an operator and continue
				return;
			}
			
			//2. Copy the local bools into the project with their new call-specific names
			std::string runstring = "r" + std::to_string(sequence.first->Callnum);
			auto& bools = sequence.first->GetLocalBools();
			auto& GraphHeads = sequence.first->GetGraphHeads();
			auto& BoolOps = sequence.first->GetBoolTypeNodes();
			
			std::map<std::string, std::string> NameChanges;
			for (auto& b : bools)
			{
				//Set new bool name (we'll rewrite the copied graph to point to these nodes)
				Proj->LocalBools[seq->Name + "@" + runstring + "@" + b.first] = b.second;
				NameChanges[b.first] = seq->Name + "@" + runstring + "@" + b.first;
			}
			
			//3. Copy the graph heads into the project (edit them to have call-specific names)
			std::map<BoolNode*, BoolNode*> GraphHeadChanges;
			Proj->GraphHeads.reserve(Proj->GraphHeads.size() + GraphHeads.size());
			for (auto gh : GraphHeads)
			{
				//Copy the graph head
				BoolNode* newgh = CopyGraph(gh);
				
				//Edit the graph head to have call-specific names
				ChangeGraphNames(newgh, NameChanges);
				
				//Add the graph head to the project
				Proj->GraphHeads.push_back(newgh);
				//Add the graph head to the map of changes
				GraphHeadChanges[gh] = newgh;
			}

			//4. Get the bool nodes and edit them to point to new graph heads
			size_t BoolNodeStart = Proj->Main.size();
			
			for (auto& b : BoolOps)
			{
				//Find out the type of bool node
				if (b->GetName() == "N_UseBool")
				{
					N_UseBool* n = new N_UseBool(*(N_UseBool*)b);
					seq->BoolNodeParams.push_back(&n->LocalFlattenedParams);
					//Edit the graph head to point to the new graph head
					if (GraphHeadChanges.contains(n->boolref))
						n->boolref = GraphHeadChanges[n->boolref];
					Proj->Main.push_back(n);
				}
				else if (b->GetName() == "N_SetBool")
				{
					N_SetBool* n = new N_SetBool(*(N_SetBool*)b);
					seq->BoolNodeParams.push_back(&n->LocalFlattenedParams);
					//Edit the graph head to point to the new graph head
					if (GraphHeadChanges.contains(n->Value))
						n->Value = GraphHeadChanges[n->Value];
					
					//Edit the name of the bool to be call-specific
					n->Bool = seq->Name + "@" + runstring + "@" + n->Bool;
					Proj->Main.push_back(n);
				}
				else if (b->GetName() == "N_SetBoolTrue")
				{
					N_SetBoolTrue* n = new N_SetBoolTrue(*(N_SetBoolTrue*)b);
					seq->BoolNodeParams.push_back(&n->LocalFlattenedParams);
					
					//Edit the name of the bool to be call-specific
					n->Name = seq->Name + "@" + runstring + "@" + n->Name;
					Proj->Main.push_back(n);
				}
			}
			
			//5. Repeat, but on each naked sequence call within the sequence (not wrapped in if statement)
			for (AST_Node* n : sequence.first->GetNodes())
			{
				if (n->GetName() == "CallSequence")
				{
					InlineSequence((CallSequence*)n, Proj);
				}
			}
		}
		
		void InlineAllSequences(Project* Proj)
		{
			for (size_t i = 0; i < Proj->Main.size(); ++i)
			{
				AST_Node* node = Proj->Main[i];
				if (node->GetName() == "CallSequence")
				{
					CallSequence* seq = (CallSequence*)node;
					InlineSequence(seq, Proj);
				}
			}

			//Now, inline all of the sequences within the if statements
			for (size_t i = 0; i < Proj->Main.size(); ++i)
			{
				AST_Node* node = Proj->Main[i];
				if (node->GetName() == "N_UseBool")
				{
					//Loop through the AST_Nodes in the body of the if statement
					N_UseBool* usebool = (N_UseBool*)node;
					for (AST_Node* n : usebool->nodes)
					{
						if (n->GetName() == "CallSequence")
						{
							CallSequence* seq = (CallSequence*)n;
							InlineSequence(seq, Proj);
						}
					}
				}
			}
		}
		
		
		void GenerateBoolGraphs(Project* Proj)
		{
			/*
			* This function has two steps. 
			* 			1. Import bool information from imports - there's only one global bool graph, so imported graphs have to be linked into
			*			   the global graph. When this happens, we mangle the names of the bools
			*			2. Have the bool allocator set bool implementations for all bools in the project.
			*/
			
			// Step 1 - import bool information from imports
			// Loop through project imports
			for (auto& imp : Proj->Namespaces)
			{
				const std::string& name = imp.first;
				Project* importproj = imp.second;

				GenerateBoolGraphs(importproj);
				//Copy bool info
				Proj->LocalBools.reserve(Proj->LocalBools.size() + importproj->LocalBools.size());
				for (auto& b : importproj->LocalBools)
				{
					Proj->LocalBools[name + "@" + b.first] = b.second;
				}

				//Copy graphs
				Proj->GraphHeads.reserve(Proj->GraphHeads.size() + importproj->GraphHeads.size());
				for (int i = 0; i < importproj->GraphHeads.size(); ++i)
				{
					Proj->GraphHeads.push_back(importproj->GraphHeads[i]);
				}

				//Copy added bool ops (if any)
				Proj->AddedBoolOps.reserve(Proj->AddedBoolOps.size() + importproj->AddedBoolOps.size());
				for (int i = 0; i < importproj->AddedBoolOps.size(); ++i)
				{
					Proj->AddedBoolOps.push_back(importproj->AddedBoolOps[i]);
				}

				//Copy bool implementations
				for (int i = 0; i < importproj->BoolImplementations.size(); ++i)
				{
					CopyImplIfNotDefined(Proj, importproj->BoolImplementations[i]);
				}
			}
			//Copy sequence data
			InlineAllSequences(Proj);

			//Now hand this off to the optimizer
			CurrentAllocator->AllocateBools(Proj);
		}
	}
}