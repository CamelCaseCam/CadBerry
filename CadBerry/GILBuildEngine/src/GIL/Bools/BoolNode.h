#pragma once
#include <gilpch.h>
#include "GIL/Parser/AST_Node.h"
#include "BoolImplementation.h"

namespace GIL
{
	namespace Parser
	{
		enum class BoolNodeType
		{
			BOOLINPUT,
			SETBOOL,
			SETBOOLTRUE,

			PLACEHOLDER,

			AND,
			OR,
			NOT,

			CAST,
			USEBOOL
		};

		class BoolNode
		{
		public:
			BoolNode() {}
			virtual ~BoolNode() {}

			virtual BoolNodeType GetType() = 0;
		};

		class N_SetBool : public AST_Node
		{
		public:
			N_SetBool(AccessNamespace&& location, std::string& Bool, BoolNode* Value) : Location(location), Value(Value), Bool(Bool) {}

			AST_Type(N_SetBool)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AccessNamespace Location;
			BoolNode* Value;
			std::string Bool;
		};
		
		class N_BoolInput : public AST_Node
		{
		public:
			N_BoolInput(std::string BoolID, Sequence* Seq, Project* origin) : BoolID(BoolID), Seq(Seq), Origin(origin) {}

			AST_Type(N_BoolInput)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			std::string BoolID;
			Sequence* Seq;
			Project* Origin;
		};

		class N_SetBoolTrue : public AST_Node
		{
		public:
			N_SetBoolTrue(AccessNamespace&& location, std::string&& name) : Location(location), Name(name) {}

			AST_Type(N_SetBoolTrue)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AccessNamespace Location;
			std::string Name;
		};

		class N_Placeholder : public BoolNode
		{
		public:
			N_Placeholder(std::string& Name, AccessNamespace&& Location) : Name(Name), Location(Location) {}
			
			virtual BoolNodeType GetType() override { return BoolNodeType::PLACEHOLDER; }
			
			std::string Name;
			AccessNamespace Location;
		};

		class N_And : public BoolNode
		{
			public:
			N_And(BoolNode* left, BoolNode* right) : left(left), right(right) {}

			virtual BoolNodeType GetType() override { return BoolNodeType::AND; }
			
			BoolNode* left;
			BoolNode* right;
		};

		class N_Or : public BoolNode
		{
			public:
			N_Or(BoolNode* left, BoolNode* right) : left(left), right(right) {}

			virtual BoolNodeType GetType() override { return BoolNodeType::OR; }
			
			BoolNode* left;
			BoolNode* right;
		};
		
		class N_Not : public BoolNode
		{
			public:
			N_Not(BoolNode* node) : node(node) {}

			virtual BoolNodeType GetType() override { return BoolNodeType::NOT; }
			
			BoolNode* node;
		};
		
		class N_Cast : public BoolNode
		{
		public:
			N_Cast(BoolImplementation* implementation, AST_Node* node) : implementation(implementation), node(node) {}

			virtual BoolNodeType GetType() override { return BoolNodeType::CAST; }
			
			BoolImplementation* implementation;
			AST_Node* node;
		};
		

		
		class N_UseBool : public AST_Node
		{
		public:
			N_UseBool(BoolNode* boolref, std::vector<AST_Node*>&& nodes);

			AST_Type(N_UseBool)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			
			BoolNode* boolref;
			BoolImplementation* implementation = nullptr;
			std::vector<AST_Node*> nodes;
		};
		std::vector<N_UseBool*> MakeIfStatements(BoolNode* boolref, std::vector<AST_Node*>& nodes, Project* project);
	}
}