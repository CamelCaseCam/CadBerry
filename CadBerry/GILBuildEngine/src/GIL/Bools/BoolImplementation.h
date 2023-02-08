#pragma once

#include "GIL/Parser/Sequence.h"
#include <gilpch.h>

namespace GIL 
{
	namespace Parser
	{
		/*
		Bool implementation code in GIL:
		ibool Name
		{
			//Uses forwards to link sequences
			Alloc => _alloc
			Dealloc => _dealloc
			
			Set => _set

			Use => _use
			NumAvailable => _numAvailable

			//Logic gates
			And => _and
			Or => _or
			Not => _not

			//Following are optional. If not defined, they will not be used
			Xor => _xor
			Nand => _nand
			Nor => _nor
			Xnor => _xnor
			
			ChainedAnd => _chainedand
			ChainedOr => _chainedor
		}
		*/
		
		/*
		Sequence parameters:
		Alloc - will be removed
		Dealloc - will be removed

		Set - name : data
		Use - inner : any

		not - input : data, output : any
		gate - left : data, right : data, output: any
		chainedgate - stub: any, val : any, result : any (result will be nothing if this isn't the last run)
		*/

		//All non-chained logic gates have two or three operands. The first two are the operands, the third is the result. The result will be 
		// high if the logic gate is true. Chained logic gates have a stub, operand, and optional result. On the last run, the result will be 
		// passed
		class BoolImplementation
		{
		public:
			BoolImplementation() {}
			BoolImplementation(std::string name) : name(name) {}
			//C++ sequence to create a bool
			Sequence* Alloc = nullptr;
			
			//C++ sequence to destroy a bool
			Sequence* Dealloc = nullptr;
			
			//Sequence (may or may not be a C++ sequence) to set a bool to true
			Sequence* Set = nullptr;
			
			//Sequence (may or may not be a C++ sequence) to condition a sequence on a bool being true
			Sequence* Use = nullptr;
			
			//Sequence to get the number of available bools. MUST be a C++ sequence and return a data type cast to int
			Sequence* NumAvailable;

			
			//Logic gate implementations
			Sequence* And = nullptr;
			Sequence* Or = nullptr;
			Sequence* Not = nullptr;
			
			bool XorAvailable = false;
			Sequence* Xor = nullptr;
			
			bool NandAvailable = false;
			Sequence* Nand = nullptr;
			
			bool NorAvailable = false;
			Sequence* Nor = nullptr;
			
			bool XnorAvailable = false;
			Sequence* Xnor = nullptr;
			
			
			//TODO: seperate sequence for back to back ANDs and ORs
			bool ChainedAndAvailable = false;
			Sequence* ChainedAnd = nullptr;
			
			bool ChainedOrAvailable = false;
			Sequence* ChainedOr = nullptr;


			//For additional sequences
			std::map<std::string, Sequence*> AdditionalSequences;
			
			//We keep a record of the implementation's name to make it possible to force a certain implementation to be used
			std::string name;

			void Save(std::ofstream& OutputFile, Parser::Project* Proj);
			static BoolImplementation* Load(std::ifstream& InputFile, Parser::Project* Proj);
		};
	}
}