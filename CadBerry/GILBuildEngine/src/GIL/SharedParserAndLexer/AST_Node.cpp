#include "gilpch.h"
#include "AST_Node.h"
#include "GIL/Lexer/Token.h"
#include "GIL/Parser/Project.h"

namespace GIL
{
	namespace Parser
	{
		Call_Params::Call_Params(std::vector<GIL::Lexer::Token*>&& Params)
		{
			this->Params.reserve(Params.size());
			for (auto token : Params)
			{
				this->Params.push_back(std::move(token->Value));
				GIL::Lexer::Token::SafeDelete(token);
			}
			Params.clear();
		}

		void Import::AddToProject(Parser::Project* Proj)
		{
			Proj->Imports.push_back(this->FileName);
		}

		void Using::AddToProject(Parser::Project* Proj)
		{
			Proj->Usings.push_back(this->FileName);
		}

		//TODO: namespaces
		void DefineSequence::AddToProject(Parser::Project* Proj)
		{

		}
		void CallParam::Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions, std::string* OutputString)
		{
		}
		void CallParam::Save(std::ofstream& OutputFile)
		{
		}
		void CallParam::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
		}
		void InheritTypedef::AddToProject(Parser::Project* Proj)
		{
		}
		void TypedefNode::AddToProject(Parser::Project* Proj)
		{
		}
		void AminoAcidLiteral::Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions, std::string* OutputString)
		{
		}
		void AminoAcidLiteral::Save(std::ofstream& OutputFile)
		{
		}
		void AminoAcidLiteral::Load(std::ifstream& InputFile, Parser::Project* Proj)
		{
		}
	}
}