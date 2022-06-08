#pragma once

#include <gilpch.h>

#include "Operation.h"
#include "Sequence.h"
#include "GIL/Modules/GILModule.h"

#include "GIL/Lexer/Token.h"
#include "GIL/Types/Type.h"

#include "Core.h"

namespace GIL
{
	namespace Parser
	{
		class GILAPI Project;
		class GILAPI Project
		{
		public:
			//The parsing step of compilation
			static Project* Parse(std::vector<Lexer::Token*>& Tokens);
			~Project();


			void Save(std::string Path);
			void Save(std::ofstream& OutputFile);    //Overload to save a project within another file
			static Project* Load(std::string Path);
			static Project* Load(std::ifstream& InputFile, std::string& DataPath);    //Overload to load a project from within a file

			std::pair<Sequence*, Parser::Project*> GetSeq(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules);    //Returns sequence based on namespaces
			Operation* GetOp(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules);    //Returns operation based on namespaces
			
			//This should probably be private
			Sequence* GetSeqFromNamespace(std::string& SeqName, std::vector<std::string*>& Namespaces, int i, std::map<std::string, GILModule*>* Modules);
			Operation* GetOpFromNamespace(std::string& OpName, std::vector<std::string*>& Namespaces, int i, std::map<std::string, GILModule*>* Modules);

			uint16_t AllocType(std::string TypeName);

			Type* GetTypeByID(uint16_t ID);
			Type* GetTypeByName(std::string& Name);

			void AddInheritance(Type* child, Type* parent);
			void AddInheritance(Type* child, uint16_t parent);
			void AddInheritance(uint16_t child, uint16_t parent);

			void RemoveInheritance(Type* child, Type* parent);
			void RemoveInheritance(Type* child, uint16_t parent);
			void RemoveInheritance(uint16_t child, uint16_t parent);

			/*
			This is another place where C++ GIL is better than the C# GIL. In the C# GIL, I implemented sequence and operation forwards (=>) by
			adding the string with the other elements's name and having the compiler treat forwards and normal elements differently. In the 
			C++ version, the different strings can point to the same operation or sequence, so there's no difference to the compiler. 
			*/
			//TODO: Save if the op or sequence is a forward in the cgil file
			std::map<std::string, Operation*> Operations;
			std::map<std::string, Sequence*> Sequences;
			std::vector<Lexer::Token*> Main;

			std::map<std::string, std::string> Attributes;
			std::vector<std::string> AvoidRSites;

			//TODO: make GIL be able to be statically or dynamically linked
			std::vector<std::string> Imports;
			std::vector<std::string> Usings;
			std::map<std::string, Project*> Namespaces;

			std::string TargetOrganism = "ERR_NO_TARGET";

			//Variables
			std::map<std::string, std::string> StrVars;
			std::map<std::string, double> NumVars;

			std::unordered_map<std::string, uint16_t> TypeName2Idx = {
				{"any", 0},
				{"cds", 1},
				{"protein", 2},
				{"ncds", 3},
				{"promoter", 4},
				{"bool", 5},
			};

			std::vector<Type*> Types = {
				&Type::any,
				&Type::cds, &Type::protein,
				&Type::ncds, &Type::promoter, &Type::GIL_bool,
			};
		};
		std::vector<GIL::Lexer::Token*> GetInsideTokens(std::vector<GIL::Lexer::Token*>& Tokens, int& i);
	}
}