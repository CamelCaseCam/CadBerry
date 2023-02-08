#include <gilpch.h>
#include "BuiltinAllocators.h"
#include "GIL/GILException.h"
#include "GIL/Errors.h"
#include "GIL/Bools/BoolNode.h"

#include "GIL/MacroUtils.h"
#include "GIL/Compiler/CompilerFunctions.h"

namespace GIL
{
	namespace Compiler
	{
		using namespace Parser;

		void FixChains(Project* Proj, BoolNode*& Head, BoolImplementation* BestImplementation);
		
		void NaiveAllocator::AllocateBools(Project* Proj)
		{
			//This allocator just picks the single best bool implementation and uses it for all bools. If it runs out of bools, it will throw an error.
			//This allocator is just a proof-of-concept. You probably shouldn't use it in production code
			
			//First, we need to find the best bool implementation
			BoolImplementation* BestImplementation = nullptr;
			int BestNumAvailable = 0;
			for (int i = 0; i < Proj->BoolImplementations.size(); ++i)
			{
				BoolImplementation* impl = Proj->BoolImplementations[i];
				//Get the number of bools available
				int numavailable = (int)impl->NumAvailable->Data(Proj);
				if (BestNumAvailable < numavailable)
				{
					BestNumAvailable = numavailable;
					BestImplementation = impl;
				}
			}

			//Now build the graph
			/*
			* There are a couple of things we need to do here:
			* 1. If this implementation doesn't support chained ANDs and ORs, we need to insert a node to split them up
			*/
			//Loop through all graph heads
			for (int i = 0; i < Proj->GraphHeads.size(); ++i)
			{
				FixChains(Proj, Proj->GraphHeads[i], BestImplementation);
			}
			
			//Make sure there are enough bools available
			if (BestNumAvailable < Proj->LocalBools.size())
			{
				CDB_BuildError(ERROR_046);
				throw GILException();
			}

			//Add the bool implementation to the bools
			for (auto b : Proj->LocalBools)
			{
				b.second->impl = BestImplementation;
			}
		}

		void FixNots(Project* Proj, BoolNode*& node, bool GraphBase = false)
		{
			switch (node->GetType())
			{
			case BoolNodeType::NOT:
			{
				//Pull the NOT node into its own graph
				N_Not* notnode = (N_Not*)node;
				if (!GraphBase)
				{
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, notnode));
					Proj->GraphHeads.push_back(notnode);
					break;
				}
				FixNots(Proj, notnode->node);
				break;
			}

			case BoolNodeType::OR:
			{
				//Call this function on both the inputs
				N_Or* ornode = (N_Or*)node;
				FixNots(Proj, ornode->left);
				FixNots(Proj, ornode->right);
				break;
			}
			case BoolNodeType::AND:
			{
				//Call this function on both the inputs
				N_And* andnode = (N_And*)node;
				FixNots(Proj, andnode->left);
				FixNots(Proj, andnode->right);
				break;
			}
			default:
				break;
			}
		}

		void FixAnds(Project* Proj, BoolNode*& node, bool GraphBase = false)
		{
			switch (node->GetType())
			{
			case BoolNodeType::NOT:
			{
				//Fix the NOT node's input
				N_Not* notnode = (N_Not*)node;
				FixAnds(Proj, notnode->node);
				break;
			}

			case BoolNodeType::OR:
			{
				//Call this function on both the inputs
				N_Or* ornode = (N_Or*)node;
				FixAnds(Proj, ornode->left);
				FixAnds(Proj, ornode->right);
				break;
			}
			case BoolNodeType::AND:
			{
				//Pull the AND node into its own graph
				N_And* andnode = (N_And*)node;

				if (!GraphBase)
				{
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, andnode));
					Proj->GraphHeads.push_back(andnode);
					break;
				}
				FixAnds(Proj, andnode->left);
				FixAnds(Proj, andnode->right);
				break;
			}
			default:
				break;
			}
		}
		
		void FixOrs(Project* Proj, BoolNode*& node, bool GraphBase = false)
		{
			switch (node->GetType())
			{
			case BoolNodeType::NOT:
			{
				//Fix the NOT node's input
				N_Not* notnode = (N_Not*)node;
				FixOrs(Proj, notnode->node);
				break;
			}

			case BoolNodeType::OR:
			{
				//Pull the OR node into its own graph
				N_And* ornode = (N_And*)node;

				if (!GraphBase)
				{
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, ornode));
					Proj->GraphHeads.push_back(ornode);
					break;
				}
				FixOrs(Proj, ornode->left);
				FixOrs(Proj, ornode->right);
				break;
			}
			case BoolNodeType::AND:
			{
				//Call this function on both the inputs
				N_And* andnode = (N_And*)node;
				FixOrs(Proj, andnode->left);
				FixOrs(Proj, andnode->right);
				break;
			}
			default:
				break;
			}
		}
		
		void FixAND_OR(Project* Proj, BoolNode*& node, bool GraphBase = false)
		{
			//We can't have a (chained or unchained) AND gate followed by an OR gate, or vice versa. Pull them into their own graphs
			switch (node->GetType())
			{
			case BoolNodeType::NOT:
			{
				//Fix the NOT node's input
				N_Not* notnode = (N_Not*)node;
				FixAND_OR(Proj, notnode->node);
				break;
			}
			case BoolNodeType::AND:
			{
				//Fix the AND node's inputs
				N_And* andnode = (N_And*)node;
				//Check if the left node is an OR
				if (andnode->left->GetType() == BoolNodeType::OR)
				{
					//Pull the AND node into its own graph
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, andnode));
					Proj->GraphHeads.push_back(andnode);
				}
				else
				{
					FixAND_OR(Proj, andnode->left);
				}

				//Check if the right node is an OR
				if (andnode->right->GetType() == BoolNodeType::OR)
				{
					//Pull the AND node into its own graph
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, andnode));
					Proj->GraphHeads.push_back(andnode);
				}
				else
				{
					FixAND_OR(Proj, andnode->right);
				}
				break;
			}
			case BoolNodeType::OR:
			{
				//Fix the OR node's inputs
				N_Or* ornode = (N_Or*)node;
				//Check if the left node is an AND
				if (ornode->left->GetType() == BoolNodeType::AND)
				{
					//Pull the OR node into its own graph
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, ornode));
					Proj->GraphHeads.push_back(ornode);
				}
				else
				{
					FixAND_OR(Proj, ornode->left);
				}

				//Check if the right node is an AND
				if (ornode->right->GetType() == BoolNodeType::AND)
				{
					//Pull the OR node into its own graph
					std::string NewBoolName = Proj->GetTempBoolName();
					node = new N_Placeholder(NewBoolName, {});
					Proj->LocalBools[NewBoolName] = (new GILBool(NewBoolName));

					Proj->AddedBoolOps.push_back(new N_SetBool({}, NewBoolName, ornode));
					Proj->GraphHeads.push_back(ornode);
				}
				else
				{
					FixAND_OR(Proj, ornode->right);
				}
				break;
			}
			default:
				break;
			}
		}

		void PullOutInput(Project* Proj, N_Placeholder* placeholder, Sequence* Input, Project* InputSource)
		{
			/*
			* From here, we have three steps:
			* 1. Get a temporary bool
			* 2. Condition the temporary bool on the input sequence (uses $InnerCode as the input parameter)
			* 3. Make placeholder point to the temporary bool
			*/

			//1
			std::string tempbool = Proj->GetTempBoolName();

			//2 - we need to make a new node tree for this
			N_BoolInput* input = new N_BoolInput(tempbool, Input, InputSource);
			Proj->LocalBools[tempbool] = new GILBool(tempbool);

			Proj->AddedBoolOps.push_back(input);

			//3
			placeholder->Location = {};
			placeholder->Name = std::move(tempbool);
		}


		void FixInputs(Project* Proj, BoolNode*& node)
		{
			//We can't have a (chained or unchained) AND gate followed by an OR gate, or vice versa. Pull them into their own graphs
			switch (node->GetType())
			{
			case BoolNodeType::NOT:
			{
				//Fix the NOT node's input
				N_Not* notnode = (N_Not*)node;
				FixInputs(Proj, notnode->node);
				break;
			}
			case BoolNodeType::AND:
			{
				//Fix the AND node's inputs
				N_And* andnode = (N_And*)node;
				FixInputs(Proj, andnode->left);
				FixInputs(Proj, andnode->right);
				break;
			}
			case BoolNodeType::OR:
			{
				//Fix the OR node's inputs
				N_Or* ornode = (N_Or*)node;
				FixInputs(Proj, ornode->left);
				FixInputs(Proj, ornode->right);
				break;
			}
			case BoolNodeType::PLACEHOLDER:
			{
				//Split the placeholder off into its own tree *if* it isn't a bool
				N_Placeholder* placeholder = (N_Placeholder*)node;
				//Get name
				std::string name;
				for (int i = 0; i < placeholder->Location.Namespaces.size(); ++i)
				{
					name += placeholder->Location.Namespaces[i] + "@";
				}
				name += placeholder->Name;

				if (!Proj->LocalBools.contains(name))
				{
					//Check if the name exists
					Call_Params dummyparams = {};
					CallSequence potentialseq = CallSequence(placeholder->Name, dummyparams, placeholder->Location);
					auto PossibleSequence = Proj->GetSeq(&potentialseq, &Modules);
					if (PossibleSequence.first == nullptr)
					{						
						//TODO: Error
						throw GILException();
					}
					PullOutInput(Proj, placeholder, PossibleSequence.first, PossibleSequence.second);
				}
				break;
			}
			default:
				break;
			}
		}

		/*All of these functions use push_back to add more graphs to the project(this is required).This function can only be called inside of 
		* an index-based for loop. You can use the foreach macro defined in MacroUtils.h for a nicer syntax
		*/
		void FixChains(Project* Proj, BoolNode*& Head, BoolImplementation* BestImplementation)
		{
			//First, split inputs off into their own bool definitions
			FixInputs(Proj, Head);

			//At the moment, we'll always have to split up NOTs
			FixNots(Proj, Head, true);
			
			//Check if we need to split up chained ANDs
			if (!BestImplementation->ChainedAndAvailable)
			{
				//a & b & c becomes bool tmp = a & b; tmp & c;
				FixAnds(Proj, Head, true);
			}
			
			//Check if we need to split up chained ORs
			if (!BestImplementation->ChainedOrAvailable)
			{
				FixOrs(Proj, Head, true);
			}
			FixAND_OR(Proj, Head);
		}
	}
}