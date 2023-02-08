#pragma once
#include <gilpch.h>
#include "GIL/Parser/AST_Node.h"
#include "BoolImplementation.h"

namespace GIL::Parser
{
	class GILAPI BoolNode;
}

template<>
struct GILAPI std::hash<GIL::Parser::BoolNode>
{
	size_t operator()(GIL::Parser::BoolNode const& bn) const noexcept;
};


namespace GIL
{
	namespace Parser
	{
		enum class GILAPI BoolNodeType
		{
			BOOLINPUT,
			SETBOOL,
			SETBOOLTRUE,

			PLACEHOLDER,
			PARAMPLACEHOLDER,

			AND,
			OR,
			NOT,

			CAST,
			USEBOOL
		};
		
		class GILAPI BoolNode
		{
		public:
			BoolNode() {}
			virtual ~BoolNode() {}
			virtual void Save(std::ofstream& OutputFile, Project* Proj) = 0;
			virtual void Load(std::ifstream& InputFile, Project* Proj) = 0;

			void SaveNode(std::ofstream& node, Project* Proj);
			static BoolNode* LoadNode(std::ifstream& node, Project* Proj);
			
			virtual size_t Hash() const noexcept = 0;

			virtual BoolNodeType GetType() const noexcept = 0;
		};

#define HashNode(...) virtual size_t Hash() const noexcept override { return ((size_t)this->GetType() __VA_ARGS__); }
#define hs(type) ) ^ std::hash<type>()(
		

		class GILAPI N_SetBool : public AST_Node
		{
		public:
			N_SetBool() {}
			N_SetBool(AccessNamespace&& location, std::string& Bool, BoolNode* Value) : Location(location), Value(Value), Bool(Bool) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;

			ReflectMe(N_SetBool, AST_Node);
			AST_Type(N_SetBool)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AccessNamespace Location;
			BoolNode* Value;
			std::string Bool;
		};
		
		class GILAPI N_BoolInput : public AST_Node
		{
		public:
			N_BoolInput() {}
			N_BoolInput(std::string BoolID, Sequence* Seq, Project* origin) : BoolID(BoolID), Seq(Seq), Origin(origin) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			ReflectMe(N_BoolInput, AST_Node);
			AST_Type(N_BoolInput);
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			std::string BoolID;
			Sequence* Seq;
			Project* Origin;
		};

		class GILAPI N_ParamBoolInput : public AST_Node
		{
		public:
			N_ParamBoolInput() {}
			N_ParamBoolInput(std::string BoolID, std::string ParamName) : BoolID(BoolID), ParamName(ParamName) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override {}
			virtual void Load(std::ifstream& InputFile, Project* Proj) override {}

			ReflectMe(N_ParamBoolInput, AST_Node);
			AST_Type(N_ParamBoolInput);
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override {}

			std::string BoolID;
			std::string ParamName;
		};

		class GILAPI N_SetBoolTrue : public AST_Node
		{
		public:
			N_SetBoolTrue() {}
			N_SetBoolTrue(AccessNamespace&& location, std::string&& name) : Location(location), Name(name) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			ReflectMe(N_SetBoolTrue, AST_Node);
			AST_Type(N_SetBoolTrue)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AccessNamespace Location;
			std::string Name;
		};

		class GILAPI N_Placeholder : public BoolNode
		{
		public:
			N_Placeholder() {}
			N_Placeholder(std::string& Name, AccessNamespace&& Location) : Name(Name), Location(Location) {}
			
			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			virtual BoolNodeType GetType() const noexcept override { return BoolNodeType::PLACEHOLDER; }


			HashNode(
				hs(std::string) Name
				hs(AccessNamespace) Location
			)
			
			std::string Name;
			AccessNamespace Location;
		};

		class GILAPI N_And : public BoolNode
		{
		public:
			N_And() {}
			N_And(BoolNode* left, BoolNode* right) : left(left), right(right) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			virtual BoolNodeType GetType() const noexcept override { return BoolNodeType::AND; }

			HashNode(
				hs(BoolNode) *left
				hs(BoolNode) *right
			)
			
			BoolNode* left;
			BoolNode* right;
		};

		class GILAPI N_Or : public BoolNode
		{
		public:
			N_Or() {}
			N_Or(BoolNode* left, BoolNode* right) : left(left), right(right) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			virtual BoolNodeType GetType() const noexcept override { return BoolNodeType::OR; }
			
			HashNode(
				hs(BoolNode)* left
				hs(BoolNode)* right
			)
			
			BoolNode* left;
			BoolNode* right;
		};
		
		class GILAPI N_Not : public BoolNode
		{
		public:
			N_Not() {}
			N_Not(BoolNode* node) : node(node) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;

			virtual BoolNodeType GetType() const noexcept override { return BoolNodeType::NOT; }
			
			HashNode(
				hs(BoolNode) *node
			)
			
			BoolNode* node;
		};
		
		class GILAPI N_Cast : public BoolNode
		{
		public:
			N_Cast() {}
			N_Cast(BoolImplementation* implementation, BoolNode* node) : implementation(implementation), node(node) {}

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;

			virtual BoolNodeType GetType() const noexcept override { return BoolNodeType::CAST; }
			
			//TODO: this needs to be improved once we implement casts
			HashNode(
				hs(BoolNode) *node
			)
			
			BoolImplementation* implementation;
			BoolNode* node;
		};
		

		
		class GILAPI N_UseBool : public AST_Node
		{
		public:
			N_UseBool() {}
			N_UseBool(BoolNode* boolref, std::vector<AST_Node*>&& nodes);

			virtual void Save(std::ofstream& OutputFile, Project* Proj) override;
			virtual void Load(std::ifstream& InputFile, Project* Proj) override;
			
			ReflectMe(N_UseBool, AST_Node);
			AST_Type(N_UseBool)
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			
			BoolNode* boolref;
			BoolImplementation* implementation = nullptr;
			std::vector<AST_Node*> nodes;
		};
		std::vector<N_UseBool*> MakeIfStatements(BoolNode* boolref, std::vector<AST_Node*>& nodes, Project* project,
			std::unordered_map<std::string, GILBool*>* LocalBools = nullptr, std::vector<BoolNode*>* GraphHeads = nullptr,
			std::vector<AST_Node*>* AddedBoolOps = nullptr);
	}
}

//Hashing stuff
inline size_t std::hash<GIL::Parser::BoolNode>::operator()(GIL::Parser::BoolNode const& bn) const noexcept
{
	return bn.Hash();
}