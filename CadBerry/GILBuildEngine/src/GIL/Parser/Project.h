#pragma once
#include <gilpch.h>

#include "Operation.h"
#include "Sequence.h"

#include "GIL/Lexer/Token.h"

namespace GIL
{
	namespace Parser
	{
		class Project
		{
		public:
			static Project* Parse(std::vector<Lexer::Token*>& Tokens);
			void Save(std::string Path);
			static Project* Load(std::string Path);

			/*
			This is another place where C++ GIL is better than the C# GIL. In the C# GIL, I implemented sequence and operation forwards (=>) by
			adding the string with the other elements's name and having the compiler treat forwards and normal elements differently. In the 
			C++ version, the different strings can point to the same operation or sequence, so there's no difference to the compiler. 
			*/
			//TODO: Save if the op or sequence is a forward in the cgil file
			std::map<std::string, Operation*> Operations;
			std::map<std::string, Sequence*> Sequences;
			std::vector<Lexer::Token*> Main;

			std::string TargetOrganism;
		};
		std::vector<GIL::Lexer::Token*> GetInsideTokens(std::vector<GIL::Lexer::Token*>& Tokens, int& i);
	}
}