#include <gilpch.h>
#include "Compiler.h"
#include "CodonEncoding.h"
#include "GIL/Lexer/Lexer.h"
#include "GIL/Lexer/LexerMacros.h"
#include "GIL/Modules/GILModule.h"
#include "CompilerMacros.h"

#include "GIL/Utils/Utils.h"
#include "GIL/RestrictionSites.h"

#include "CadBerry.h"
#include "CadBerry/SharedLib.h"

#include "CompilerFunctions.h"

namespace GIL
{
	int CurrentSequenceCallDepth = 0;

	namespace Compiler
	{
		using namespace GIL::Lexer;
		using namespace GIL::Parser;

		std::pair<std::vector<Region>, std::string> Compile(Project* Proj, std::vector<AST_Node*>* Nodes, std::string* Distribution)
		{
			if (Nodes == nullptr)
			{
				Nodes = &Proj->Main;
			}
			std::vector<Region> Output;
			std::string Code;
			CodonEncoding CurrentEncoding(Proj->TargetOrganism);
			std::vector<Region> OpenRegions = {
				Region("Main", 1, -1)
			};

			CompilerContext Context = { Nodes, &Output, &Code, &CurrentEncoding, nullptr, 0, &OpenRegions, Distribution };

			//Import any imports
			ImportAllProjectImports(Proj, Proj);

			LinkAllProjectDLLs(Proj);

			//Initialize all operators and forwards
			InitForwardsAndOperators(Proj);
			

			//Compile the nodes
			for (Context; Context.NodeIdx < Context.Nodes->size(); ++Context.NodeIdx)
			{
				(*Context.Nodes)[Context.NodeIdx]->Compile(Context, Proj);
			}

			return { std::move(Output), std::move(Code) };
		}


		inline void ImportAllProjectOperators(Project* Proj, Project* global)
		{
			InitForwardsAndOperators(Proj);
			//Copy all the operators from the local project to the global project
			for (auto& op : Proj->Operators)
			{
				if (global->Operators.contains(op.first))
				{
					//Replace the operator, keeping the current implementation as an alternate
					Operator* LastImplementation = ((Operator*)op.second)->GetLastImplementation();
					LastImplementation->AlternateImplementation = (Operator*)global->Operators[op.first];

					//Replace the operator in the global project
					global->Operators[op.first] = op.second;
				}
			}
		}

		void ImportFile(std::string& Path, Project* Proj, Project* global)
		{
			std::filesystem::path path = std::filesystem::path(Path);
			bool imported = false;

			if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
				+ path.stem().string() + ".cgil"))    //if cached/precompiled file exists
			{
				Project* p = Project::Load(CDB::Application::Get().PreBuildDir + "\\" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				Proj->Namespaces[path.stem().string()] = p;

				//All operators are global
				InitForwardsAndOperators(Proj);
				ImportAllProjectOperators(p, global);
				ImportAllProjectImports(p, global);
				LinkAllProjectDLLs(p);
				imported = true;
			}
			else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\" 
				+ path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
			{
				Project* p = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/" 
					+ path.parent_path().string() + "\\" + path.stem().string() + ".cgil");
				Proj->Namespaces[path.stem().string()] = p;

				//All operators are global
				InitForwardsAndOperators(Proj);
				ImportAllProjectOperators(p, global);
				ImportAllProjectImports(p, global);
				LinkAllProjectDLLs(p);
				imported = true;
			}
			else if (std::filesystem::exists(CDB::Application::Get().OpenProject->Path + "\\" + path.parent_path().string() + "\\" + path.stem().string() + ".gil"))
			{
				std::ifstream t(CDB::Application::Get().OpenProject->Path + "\\" + path.parent_path().string() + "\\" 
					+ path.stem().string() + ".gil");
				std::stringstream ss;
				ss << t.rdbuf();

				std::string GILsrc = ss.str();

				auto Tokens = *GIL::Lexer::Tokenize(GILsrc);
				Project* p = GIL::Parser::Project::Parse(Tokens);
				p->Save(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
					+ path.stem().string() + ".cgil");

				Proj->Namespaces[path.stem().string()] = p;

				//All operators are global
				InitForwardsAndOperators(Proj);
				ImportAllProjectOperators(p, global);
				ImportAllProjectImports(p, global);
				LinkAllProjectDLLs(p);
				imported = true;
			}

			if (Proj->TargetOrganism != "")
			{
				if (std::filesystem::exists(CDB::Application::Get().PreBuildDir + "\\" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //if cached/precompiled file exists
				{
					Project* p = Project::Load(CDB::Application::Get().PreBuildDir + "\\"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					Proj->Namespaces[path.stem().string()] = p;

					//All operators are global
					InitForwardsAndOperators(Proj);
					ImportAllProjectOperators(p, global);
					ImportAllProjectImports(p, global);
					LinkAllProjectDLLs(p);
					imported = true;
				}
				else if (std::filesystem::exists(CDB::Application::Get().PathToEXE + "/Build/Libs/" + path.parent_path().string() + "\\"
					+ Proj->TargetOrganism + "@" + path.stem().string() + ".cgil"))    //If a compiled file in the libs folder exists
				{
					Project* p = Project::Load(CDB::Application::Get().PathToEXE + "/Build/Libs/"
						+ path.parent_path().string() + "\\" + Proj->TargetOrganism + "@" + path.stem().string() + ".cgil");
					Proj->Namespaces[path.stem().string()] = p;

					//All operators are global
					InitForwardsAndOperators(Proj);
					ImportAllProjectOperators(p, global);
					ImportAllProjectImports(p, global);
					LinkAllProjectDLLs(p);
					imported = true;
				}
			}

			if (!imported)
			{
				CDB_BuildError("Could not find file \"{0}\"", path.string());
			}
		}

		//Prepro conditions
		bool Prepro_IsEq(Project* Proj, std::vector<Token*>* Tokens)
		{
			if (Tokens->size() != 2)
			{
				CDB_BuildError("#If IsEq expected two operands");
				for (Token* t : *Tokens)
				{
					CDB_BuildInfo(*t);
				}
				return false;
			}

			if (Proj->StrVars.contains((*Tokens)[0]->Value) && Proj->StrVars.contains((*Tokens)[1]->Value))
			{
				return Proj->StrVars[(*Tokens)[0]->Value] == Proj->StrVars[(*Tokens)[1]->Value];
			}
			else if (Proj->NumVars.contains((*Tokens)[0]->Value) && Proj->NumVars.contains((*Tokens)[1]->Value))
			{
				return Proj->NumVars[(*Tokens)[0]->Value] == Proj->NumVars[(*Tokens)[1]->Value];
			}
			else
			{
				CDB_BuildError("Variables \"{0}\" and \"{1}\" either are not the same type or do not exist", (*Tokens)[0]->Value, 
					(*Tokens)[1]->Value);
				return false;
			}
		}
	}
}