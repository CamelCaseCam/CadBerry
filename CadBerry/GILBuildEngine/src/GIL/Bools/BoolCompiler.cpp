#include <gilpch.h>
#include "BoolCompiler.h"
#include "BuiltinAllocators.h"

namespace GIL
{
	namespace Compiler
	{
		BoolAllocator* NaiveAllocatorInstance = new NaiveAllocator();
		std::unordered_map<std::string, BoolAllocator*> boolAllocators = {
			{ "NaiveAllocator",  NaiveAllocatorInstance},
		};
		BoolAllocator* CurrentAllocator = NaiveAllocatorInstance;

		using namespace Parser;
		void GenerateBoolGraphs(Project* Proj)
		{
			/*
			* This function has two steps. 
			* 			1. Import bool information from imports - there's only one global bool graph, so imported graphs have to be linked into
			*			   the global graph. When this happens, we mangle the 
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
			}

			//Now hand this off to the optimizer
			CurrentAllocator->AllocateBools(Proj);
		}
	}
}