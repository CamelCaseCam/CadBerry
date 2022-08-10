#pragma once
#include <gilpch.h>
#include "GIL/Parser/Project.h"
#include "CodonEncoding.h"
#include "GIL/Errors.h"

namespace GIL
{
	namespace Compiler
	{
		void AminosToDNA(std::string& DNA, std::string& aminos, Parser::Project* Proj, CodonEncoding& CurrentEncoding);
		void AminosIDXToDNA(std::string& DNA, std::string& aminos, std::vector<int>& Idxs, Parser::Project* Proj, CodonEncoding& CurrentEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Parser::Project* Proj, CodonEncoding& OriginEncoding);
		void DNA2Aminos(std::string& Aminos, std::vector<int>& AminoIdxs, std::string& DNA, Parser::Project* Proj);
		void ImportFile(std::string& Path, Parser::Project* Proj, Parser::Project* global);
		void LinkDLL(std::string& Path, Parser::Project* Proj);
		bool HasRestrictionSites(std::string& DNA, std::string& Codon, Parser::Project* Proj);



		extern std::map<std::string, GILModule*> Modules;
		GILAPI extern std::map<std::string, std::function<bool(Parser::Project*, std::vector<Lexer::Token*>*)>> PreproConditions;


#ifdef CDB_PLATFORM_WINDOWS
		typedef GILModule* (__stdcall* f_GetModule)();
#else
		typedef GILModule* (*f_GetModule)();
#endif


		//Inline functions

		inline void AddRegionToVector(const Parser::Region& reg, std::vector<Parser::Region>& vec, int& last)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
				--last;
			}
		}

		inline void AddRegionToVector(const Parser::Region&& reg, std::vector<Parser::Region>& vec, int& last)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
				--last;
			}
		}

		inline void AddRegionToVector(const Parser::Region& reg, std::vector<Parser::Region>& vec)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
			}
		}

		inline void AddRegionToVector(const Parser::Region&& reg, std::vector<Parser::Region>& vec)
		{
			if (reg.Start != reg.End && reg.End != 0)
			{
				vec.push_back(reg);
			}
		}

		//Imports recursively
		inline void ImportAllProjectImports(Parser::Project* Proj, Parser::Project* global)
		{
			for (std::string& s : Proj->Imports)
			{
				if (!Proj->Namespaces.contains(s))
				{
					ImportFile(s, Proj, global);
				}
			}
		}

		//Links recursively
		inline void LinkAllProjectDLLs(Parser::Project* Proj)
		{
			for (std::string& s : Proj->Usings)
			{
				LinkDLL(s, Proj);
			}
		}

		inline void InitForwardsAndOperators(Parser::Project* Proj)
		{
			//Initialize forwards
			for (auto seq : Proj->Sequences)
				seq.second->CompileTimeInit(Proj);
			
			//Initialize operators
			for (auto op : Proj->Operators)
			{
				Operator* _op = (Operator*)op.second;
				if (Proj->Sequences.contains(_op->DestinationName))
				{
					_op->DestinationSequence = Proj->Sequences[_op->DestinationName];
					_op->Origin = Proj;
					_op->ParameterTypes = _op->DestinationSequence->ParameterTypes;
					_op->ParamIdx2Name = _op->DestinationSequence->ParamIdx2Name;
					_op->SeqType = _op->DestinationSequence->SeqType;
				}
				else
				{
					CDB_BuildError(ERROR_033, op.first, _op->DestinationName);
				}
			}
		}
	}
}