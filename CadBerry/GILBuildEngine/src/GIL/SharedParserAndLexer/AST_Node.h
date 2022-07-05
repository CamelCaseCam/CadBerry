#pragma once
#include <gilpch.h>

namespace GIL
{
	namespace Parser
	{
		class Region;
		class Project;
	}
	
	namespace Lexer
	{
		class Token;
	}
	
#define NoCompile 

#define AST_Type(Type) virtual std::string GetName() override { return #Type; }

#define ParserNode() void AddToProject(Parser::Project* Proj)
	
#define Concat_exp(a, b) a ## b
#define Concat(a, b) Concat_exp(a, b)
	

	namespace Parser
	{
		/*
These macros allow simple reflection so we can load a node as the right type.The nodes can save their name as a string when saving the node.
When we load the node from the file, we can use the name to find the right type.
*/

//Creates the map
#define ReflectableClass(Parent) \
inline std::unordered_map<std::string, Parent* (*)()> _Reflectable_ ## Parent ## _Map; \
struct _Reflectable_MapAdder_ ## Parent\
{\
	_Reflectable_MapAdder_ ## Parent (std::string Name, Parent* (*ChildCreator)()) { _Reflectable_ ## Parent ## _Map[Name] = ChildCreator;}\
};\
class Parent

#define ReflectMe(ClassName, Parent)\
static Parent* GetImplementation() { return new ClassName (); }\
inline static const _Reflectable_MapAdder_ ## Parent    _Reflectable_ ## ClassName ## _ ## Parent = _Reflectable_MapAdder_ ## Parent(#ClassName, GetImplementation)\


		ReflectableClass(AST_Node)
		{
		public:
			//TODO
			virtual ~AST_Node() {}

			virtual void Compile(std::vector<AST_Node*>&Nodes, Parser::Project * Project, std::vector<Parser::Region>*OutputRegions,
				std::string * OutputString) {}

			virtual void Save(std::ofstream & OutputFile) { }

			//Loads including type information
			static AST_Node* LoadNode(std::ifstream & InputFile, Parser::Project * Proj);
			virtual void Load(std::ifstream & InputFile, Parser::Project * Proj) {}

			virtual std::string GetName() { return ""; }
		};

		//I've changed this so the new type syntax is $Name [: type]
		class ParamNode : public AST_Node
		{
		public:
			ParamNode(std::string&& Name, std::string&& Type) : Name(Name), Type(Type) {}
			ParamNode(std::string&& Name) : Name(Name), Type("any") {}
			std::string Name;
			std::string Type;
			std::string Type;

			NoCompile
			AST_Type(Param)
		};

		//Params closes OpenParams
		class Params : public AST_Node
		{
		public:
			Params() {}
			Params(std::vector<ParamNode*>&& Params) : m_Params(Params) {}
			Params(std::vector<ParamNode*>&& Params, std::vector<std::string>&& Types) : m_Params(Params) {}

			NoCompile
				AST_Type(Params)

			std::vector<ParamNode*> m_Params;
		};


		//__________________________________________________________________________________________________
		// import, include, and using
		//__________________________________________________________________________________________________

		//Links to CGIL or GIL file
		class Import : public AST_Node
		{
		public:
			//For reflection
			Import(std::string&& filename) : FileName(filename) {}

			NoCompile
				AST_Type(Import)

				void AddToProject(Parser::Project* Proj);

			std::string FileName;
		};


		//Links to dynamic library
		class Using : public AST_Node
		{
		public:
			Using(std::string&& filename) : FileName(filename) {}

			NoCompile
				AST_Type(Import)

				void AddToProject(Parser::Project* Proj);

			std::string FileName;
		};


		//__________________________________________________________________________________________________
		// from and for
		//__________________________________________________________________________________________________


		class From : public AST_Node
		{
		public:
			From() {}
			From(std::string&& Origin, std::vector<AST_Node*>&& Body) : Origin(Origin), Body(Body) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString);

			AST_Type(From)
				ReflectMe(From, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;


			std::string Origin;
			std::vector<AST_Node*> Body;
		};

		class For : public AST_Node
		{
		public:
			For() {}
			For(std::string&& Target, std::vector<AST_Node*>&& Body) : Target(Target), Body(Body) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString);

			AST_Type(For)
				ReflectMe(For, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Target;
			std::vector<AST_Node*> Body;
		};

		class DefineSequence : public AST_Node
		{
		public:
			DefineSequence(std::string&& Name, Params&& Params, std::string&& Type, std::vector<AST_Node*>&& Body) : Name(Name), m_Params(Params),
				Type(Type), Body(Body) {}

			void AddToProject(Parser::Project* Proj);

			NoCompile
				AST_Type(DefineSequence)

				std::string Name;
			Params m_Params;
			std::string Type;

			std::vector<AST_Node*> Body;
		};

		class DefineOperation : public AST_Node
		{
		public:
			DefineOperation(std::string&& Name, Params&& Params, std::string&& Type, std::vector<AST_Node*>&& Body) : Name(Name), m_Params(Params),
				Type(Type), Body(Body) {}

			void AddToProject(Parser::Project* Proj);

			NoCompile
				AST_Type(DefineSequence)

				std::string Name;
			Params m_Params;
			std::string Type;

			std::vector<AST_Node*> Body;
		};


		//__________________________________________________________________________________________________
		// Forwards
		//__________________________________________________________________________________________________

		class Forward : public AST_Node
		{
		public:
			Forward(std::string&& Origin, std::string&& Destination) : Origin(Origin), Destination(Destination) {}

			void AddToProject(Parser::Project* Proj);

			NoCompile
				AST_Type(Forward)

				std::string Origin;
			std::string Destination;
		};

		class DefineOperator : public AST_Node
		{
		public:
			DefineOperator(std::string&& Name, std::string&& Implementation) : Name(Name), Implementation(Implementation) {}

			void AddToProject(Parser::Project* Proj);

			NoCompile
				AST_Type(Forward)

				std::string Name;
			std::string Implementation;
		};


		//__________________________________________________________________________________________________
		// Forwards
		//__________________________________________________________________________________________________

		class Call_Params : public AST_Node
		{
		public:
			Call_Params() {}
			Call_Params(std::vector<std::string>&& Params) : Params(Params) {}
			Call_Params(std::vector<GIL::Lexer::Token*>&& Params);

			NoCompile
				AST_Type(Call_Params)

				std::vector<std::string> Params;
		};

		class CallSequence : public AST_Node
		{
		public:
			CallSequence() {}
			CallSequence(std::string&& Name, Call_Params&& Params) : Name(Name), Params(std::move(Params.Params)) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString);
			AST_Type(CallSequence)
				ReflectMe(CallSequence, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Name;
			std::vector<std::string> Params;
		};

		class CallOperation : public AST_Node
		{
		public:
			CallOperation() {}
			CallOperation(std::string&& Name, Call_Params&& Params, std::vector<AST_Node*>&& InnerNodes) : Name(Name), Params(std::move(Params.Params)),
				InnerNodes(InnerNodes) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;
			AST_Type(CallOperation)
				ReflectMe(CallOperation, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Name;
			std::vector<std::string> Params;
			std::vector<AST_Node*> InnerNodes;
		};


		//__________________________________________________________________________________________________
		// Preprocessor directives
		//__________________________________________________________________________________________________

		class SetTarget : public AST_Node
		{
		public:
			SetTarget(std::string&& Target) : Target(Target) {}

			NoCompile
				AST_Type(SetTarget)

				void AddToProject(Parser::Project* Proj);

			std::string Target;
		};

		class BeginRegion : public AST_Node
		{
		public:
			BeginRegion() {}
			BeginRegion(std::string&& RegionName) : RegionName(RegionName) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;
			AST_Type(BeginRegion)
				ReflectMe(BeginRegion, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string RegionName;
		};

		class EndRegion : public AST_Node
		{
		public:
			EndRegion() {}
			EndRegion(std::string&& RegionName) : RegionName(RegionName) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;
			AST_Type(BeginRegion)
				ReflectMe(EndRegion, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string RegionName;
		};

		class SetAttr : public AST_Node
		{
		public:
			SetAttr() {}
			SetAttr(std::string&& AttrName, std::string&& AttrValue) : AttrName(AttrName), AttrValue(AttrValue) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;
			AST_Type(SetAttr)
				ReflectMe(SetAttr, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string AttrName;
			std::string AttrValue;
		};

		enum class VariableType
		{
			str,
			num
		};

		std::unordered_map<std::string, VariableType> VariableTypes = {
			{ "str", VariableType::str },
			{ "num", VariableType::num }
		};

		class SetVar : public AST_Node
		{
		public:
			SetVar() {}
			SetVar(std::string& variableType, std::string&& VarName, std::string&& VarValue) : m_VariableType(VariableTypes[variableType]),
				VarName(VarName), VarValue(VarValue) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(SetVar)
				ReflectMe(SetVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			VariableType m_VariableType;
			std::string VarName;
			std::string VarValue;
		};

		class IncVar : public AST_Node
		{
		public:
			IncVar() {}
			IncVar(std::string&& VarName) : VarName(VarName) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(IncVar)
				ReflectMe(IncVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string VarName;
		};

		class DecVar : public AST_Node
		{
		public:
			DecVar() {}
			DecVar(std::string&& VarName) : VarName(VarName) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(DecVar)
				ReflectMe(DecVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string VarName;
		};

		class Prepro_If : public AST_Node
		{
		public:
			Prepro_If() {}
			Prepro_If(std::string&& Condition, std::vector<Lexer::Token*>&& Parameters, std::vector<AST_Node*>&& Body) : Condition(Condition),
				Params(Parameters), Body(Body) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(Prepro_If)
			ReflectMe(Prepro_If, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Condition;
			std::vector<Lexer::Token*> Params;
			std::vector<AST_Node*> Body;
		};

		class Prepro_Else : public AST_Node
		{
		public:
			Prepro_Else() {}
			Prepro_Else(std::vector<AST_Node*>&& Body) : Body(Body) {}

			NoCompile
			AST_Type(Prepro_Else)
			ReflectMe(Prepro_Else, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			//This is called when an if statement evaluates to false. Compiles the body of the else statement.
			void Evaluate(std::vector<AST_Node>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString);

			std::vector<AST_Node*> Body;
		};


		//__________________________________________________________________________________________________
		// DNA and amino acid literals
		//__________________________________________________________________________________________________
		class DNALiteral : public AST_Node
		{
		public:
			DNALiteral() {}
			DNALiteral(std::string&& Literal) : Literal(Literal) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(DNALiteral)
			ReflectMe(DNALiteral, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Literal;
		};

		class AminoAcidLiteral : public AST_Node
		{
		public:
			AminoAcidLiteral() {}
			AminoAcidLiteral(std::string&& Literal) : Literal(Literal) {}

			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;

			AST_Type(AminoAcidLiteral)
			ReflectMe(AminoAcidLiteral, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Literal;
		};


		//__________________________________________________________________________________________________
		// Typedefs
		//__________________________________________________________________________________________________
		class TypedefNode : public AST_Node
		{
		public:
			TypedefNode(std::string&& Name) : Name(Name) {}
			TypedefNode(std::string&& Name, std::string Inherits) : Name(Name), Inherits(Inherits) {}

			NoCompile
			AST_Type(TypedefNode)
			void AddToProject(Parser::Project* Proj);

			std::string Name;
			std::string Inherits = "any";
		};

		class InheritTypedef : public AST_Node
		{
		public:
			InheritTypedef(std::string&& Type, std::string&& Parent) : Type(Type), Parent(Parent) {}

			NoCompile
			AST_Type(InheritTypedef)
			void AddToProject(Parser::Project* Proj);

			std::string Type;
			std::string Parent;
		};


		//Comment class (doesn't do anything)
		class Comment : public AST_Node
		{
			NoCompile
			AST_Type(Comment)
		};



		//For calling params passed to sequence
		class CallParam : public AST_Node
		{
		public:
			CallParam() {}
			CallParam(std::string&& ParamName, Params&& Params) : ParamName(ParamName), Params(Params) {}
			
			virtual void Compile(std::vector<AST_Node*>& Nodes, Parser::Project* Project, std::vector<Parser::Region>* OutputRegions,
				std::string* OutputString) override;
			
			AST_Type(CallParam)
			ReflectMe(CallParam, AST_Node);
			
			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;
			
			std::string ParamName;
			Params Params;
		};
	}
}