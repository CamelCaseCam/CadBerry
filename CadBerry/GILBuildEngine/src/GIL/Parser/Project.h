#pragma once
#include <gilpch.h>

#include "Operation.h"
#include "Sequence.h"
#include "GIL/Modules/GILModule.h"

#include "GIL/Lexer/Token.h"

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

			Sequence* GetSeq(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules);    //Returns sequence based on namespaces
			Operation* GetOp(std::vector<Lexer::Token*>* Tokens, int& i, std::map<std::string, GILModule*>* Modules);    //Returns operation based on namespaces
			
			//This should probably be private
			Sequence* GetSeqFromNamespace(std::string& SeqName, std::vector<std::string*>& Namespaces, int i, std::map<std::string, GILModule*>* Modules);
			Operation* GetOpFromNamespace(std::string& OpName, std::vector<std::string*>& Namespaces, int i, std::map<std::string, GILModule*>* Modules);

			/*
			This is another place where C++ GIL is better than the C# GIL. In the C# GIL, I implemented sequence and operation forwards (=>) by
			adding the string with the other elements's name and having the compiler treat forwards and normal elements differently. In the 
			C++ version, the different strings can point to the same operation or sequence, so there's no difference to the compiler. 
			*/
			//TODO: Save if the op or sequence is a forward in the cgil file
			std::map<std::string, Operation*> Operations;
			std::map<std::string, Sequence*> Sequences;
			std::vector<Lexer::Token*> Main;

			//TODO: make GIL be able to be statically or dynamically linked
			std::vector<std::string> Imports;
			std::vector<std::string> Usings;
			std::map<std::string, Project*> Namespaces;

			std::string TargetOrganism;
		};
		std::vector<GIL::Lexer::Token*> GetInsideTokens(std::vector<GIL::Lexer::Token*>& Tokens, int& i);
	}
}