#pragma once
#include <gilpch.h>
#include "Core.h"

namespace GIL
{
	namespace Parser
	{
		class Region;
		class Project;
		class AST_Node;
	}
	class Param;
	
	namespace Lexer
	{
		class Token;
	}
	

	
	namespace Compiler
	{
		class CodonEncoding;
		
		//This struct DOES NOT own any of the pointers it points to.
		struct CompilerContext
		{
			std::vector<Parser::AST_Node*>* Nodes;
			std::vector<Parser::Region>* OutputRegions;
			std::string* OutputString;
			CodonEncoding* Encoding;
			std::map<std::string, Param>* Params = nullptr;
			size_t NodeIdx = 0;

			std::vector<Parser::Region>* OpenRegions;
		};
	}

	class Sequence;
	
#define NoCompile 

#define AST_Type(Type) virtual std::string GetName() override { return #Type; }
	
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
class GILAPI Parent

#define ReflectMe(ClassName, Parent)\
static Parent* GetImplementation() { return new ClassName (); }\
inline static const _Reflectable_MapAdder_ ## Parent    _Reflectable_ ## ClassName ## _ ## Parent = _Reflectable_MapAdder_ ## Parent(#ClassName, GetImplementation)\

		struct ParserPosition
		{
			size_t Line;

			//TODO
			size_t Column;
		};

		ReflectableClass(AST_Node)
		{
		public:
			//TODO
			virtual ~AST_Node() {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) {}

			virtual void Save(std::ofstream & OutputFile) { }

			//Loads including type information
			static AST_Node* LoadNode(std::ifstream & InputFile, Parser::Project * Proj);
			static void SaveNode(AST_Node* Node, std::ofstream& OutputFile);
			virtual void Load(std::ifstream & InputFile, Parser::Project * Proj) {}

			virtual std::string GetName() { return ""; }

			virtual bool CanAddToProject() { return false; }

			void SetPos(ParserPosition newPos) { pos = newPos; }

			ParserPosition pos;
		};

		class GILAPI ProjectNode : public AST_Node
		{
		public:
			virtual bool CanAddToProject() override { return true; }

			virtual void AddToProject(Parser::Project* Proj) = 0;
		};

		//I've changed this so the new type syntax is $Name [: type]
		class GILAPI ParamNode : public AST_Node
		{
		public:
			ParamNode(std::string&& Name, std::string&& Type) : Name(Name), Type(Type) {}
			ParamNode(std::string&& Name) : Name(Name), Type("any") {}
			std::string Name;
			std::string Type;
			
			NoCompile
			AST_Type(Param)
		};

		//Params closes OpenParams
		class GILAPI Params : public AST_Node
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
		class GILAPI Import : public ProjectNode
		{
		public:
			//For reflection
			Import(std::string&& filename) : FileName(filename) {}

			NoCompile
			AST_Type(Import)

			virtual void AddToProject(Parser::Project* Proj) override;

			std::string FileName;
		};


		//Links to dynamic library
		class GILAPI Using : public ProjectNode
		{
		public:
			Using(std::string&& filename) : FileName(filename) {}

			NoCompile
				AST_Type(Import)

				virtual void AddToProject(Parser::Project* Proj) override;

			std::string FileName;
		};


		//__________________________________________________________________________________________________
		// from and for
		//__________________________________________________________________________________________________


		class GILAPI From : public AST_Node
		{
		public:
			From() {}
			From(std::string&& Origin, std::vector<AST_Node*>&& Body) : Origin(Origin), Body(Body) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(From)
			ReflectMe(From, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;


			std::string Origin;
			std::vector<AST_Node*> Body;
		};

		class GILAPI For : public AST_Node
		{
		public:
			For() {}
			For(std::string&& Target, std::vector<AST_Node*>&& Body) : Target(Target), Body(Body) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(For)
			ReflectMe(For, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Target;
			std::vector<AST_Node*> Body;
		};

		class GILAPI DefineSequence : public ProjectNode
		{
		public:
			DefineSequence(std::string&& Name, Params&& Params, std::string&& Type, std::vector<AST_Node*>&& Body) : Name(Name), m_Params(Params),
				Type(Type), Body(Body) {}

			virtual void AddToProject(Parser::Project* Proj) override;

			NoCompile
			AST_Type(DefineSequence)

			std::string Name;
			Params m_Params;
			std::string Type;

			std::vector<AST_Node*> Body;
		};

		class GILAPI DefineOperation : public ProjectNode
		{
		public:
			DefineOperation(std::string&& Name, Params&& Params, std::string&& Type, std::vector<AST_Node*>&& Body) : Name(Name), m_Params(Params),
				Type(Type), Body(Body) {}

			virtual void AddToProject(Parser::Project* Proj) override;

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

		class GILAPI Forward : public ProjectNode
		{
		public:
			Forward(std::string&& Origin, std::string&& Destination) : Origin(Origin), Destination(Destination) {}
			Forward(std::string& Origin, std::string& Destination) : Origin(Origin), Destination(Destination) {}

			virtual void AddToProject(Parser::Project* Proj) override;

			NoCompile
			AST_Type(Forward)

			std::string Origin;
			std::string Destination;
		};

		class GILAPI DefineOperator : public ProjectNode
		{
		public:
			DefineOperator(std::string&& Name, std::string&& Implementation) : Name(Name), Implementation(Implementation) {}

			virtual void AddToProject(Parser::Project* Proj) override;

			NoCompile
			AST_Type(Forward)

			std::string Name;
			std::string Implementation;
		};


		//__________________________________________________________________________________________________
		// Forwards
		//__________________________________________________________________________________________________

		class GILAPI Call_Params : public AST_Node
		{
		public:
			Call_Params() {}
			Call_Params(std::vector<std::string>&& Params) : Params(Params) {}
			Call_Params(std::vector<GIL::Lexer::Token*>&& Params);

			NoCompile
			AST_Type(Call_Params)
			
			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::vector<std::string> Params;
			
			std::map<std::string, Param> ToParamMap(Compiler::CompilerContext& Context, Project* Project, std::vector<std::string>& ParamIdx2Name);
		};

		class GILAPI AccessNamespace : public AST_Node
		{
		public:
			AccessNamespace() {}
			AccessNamespace(std::vector<Lexer::Token*>&& namespaces);

			NoCompile
			AST_Type(AccessNamespace)

			std::vector<std::string> Namespaces;
		};

		class GILAPI CallSequence : public AST_Node
		{
		public:
			CallSequence() {}
			CallSequence(std::string&& Name, Call_Params&& Params, AccessNamespace&& Location) : Name(Name), Params(Params),
				Location(std::move(Location.Namespaces)) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			AST_Type(CallSequence)
			ReflectMe(CallSequence, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Name;
			std::vector<std::string> Location;
			Call_Params Params;
		};

		class GILAPI CallOperation : public AST_Node
		{
		public:
			CallOperation() {}
			CallOperation(std::string&& Name, Call_Params&& Params, std::vector<AST_Node*>&& InnerNodes, AccessNamespace&& Location) : Name(Name), 
				Params(Params), InnerNodes(InnerNodes), Location(std::move(Location.Namespaces)) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			AST_Type(CallOperation)
			ReflectMe(CallOperation, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Name;
			Call_Params Params;
			std::vector<AST_Node*> InnerNodes;
			std::vector<std::string> Location;
		};

		//__________________________________________________________________________________________________
		// Preprocessor directives
		//__________________________________________________________________________________________________

		class GILAPI SetTarget : public ProjectNode
		{
		public:
			SetTarget(std::string&& Target) : Target(Target) {}

			NoCompile
				AST_Type(SetTarget)

				virtual void AddToProject(Parser::Project* Proj) override;

			std::string Target;
		};

		class GILAPI BeginRegion : public AST_Node
		{
		public:
			BeginRegion() {}
			BeginRegion(std::string&& RegionName) : RegionName(RegionName) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			AST_Type(BeginRegion)
			ReflectMe(BeginRegion, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string RegionName;
		};

		class GILAPI EndRegion : public AST_Node
		{
		public:
			EndRegion() {}
			EndRegion(std::string&& RegionName) : RegionName(RegionName) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			AST_Type(BeginRegion)
				ReflectMe(EndRegion, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string RegionName;
		};

		class GILAPI SetAttr : public AST_Node
		{
		public:
			SetAttr() {}
			SetAttr(std::string&& AttrName, std::string&& AttrValue) : AttrName(AttrName), AttrValue(AttrValue) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			AST_Type(SetAttr)
			ReflectMe(SetAttr, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string AttrName;
			std::string AttrValue;
		};

		enum class GILAPI VariableType
		{
			str,
			num
		};

		extern std::unordered_map<std::string, VariableType> VariableTypes;

		class GILAPI SetVar : public AST_Node
		{
		public:
			SetVar() {}
			SetVar(std::string& variableType, std::string&& VarName, std::string&& VarValue) : m_VariableType(VariableTypes[variableType]),
				VarName(VarName), VarValue(VarValue) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(SetVar)
				ReflectMe(SetVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			VariableType m_VariableType;
			std::string VarName;
			std::string VarValue;
		};

		class GILAPI IncVar : public AST_Node
		{
		public:
			IncVar() {}
			IncVar(std::string&& VarName) : VarName(VarName) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(IncVar)
				ReflectMe(IncVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string VarName;
		};

		class GILAPI DecVar : public AST_Node
		{
		public:
			DecVar() {}
			DecVar(std::string&& VarName) : VarName(VarName) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(DecVar)
				ReflectMe(DecVar, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string VarName;
		};

		class GILAPI Prepro_If : public AST_Node
		{
		public:
			Prepro_If() {}
			Prepro_If(std::string&& Condition, std::vector<Lexer::Token*>&& Parameters, std::vector<AST_Node*>&& Body) : Condition(Condition),
				Params(Parameters), Body(Body) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(Prepro_If)
			ReflectMe(Prepro_If, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Condition;
			std::vector<Lexer::Token*> Params;
			std::vector<AST_Node*> Body;
		};

		class GILAPI Prepro_Else : public AST_Node
		{
		public:
			Prepro_Else() {}
			Prepro_Else(std::vector<AST_Node*>&& Body) : Body(Body) {}

			NoCompile
			AST_Type(Prepro_Else)
			ReflectMe(Prepro_Else, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::vector<AST_Node*> Body;
		};


		//__________________________________________________________________________________________________
		// DNA and amino acid literals
		//__________________________________________________________________________________________________
		class GILAPI DNALiteral : public AST_Node
		{
		public:
			DNALiteral() {}
			DNALiteral(std::string&& Literal) : Literal(Literal) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(DNALiteral)
			ReflectMe(DNALiteral, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Literal;
		};

		class GILAPI AminoAcidLiteral : public AST_Node
		{
		public:
			AminoAcidLiteral() {}
			AminoAcidLiteral(std::string&& Literal) : Literal(Literal) {}

			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;

			AST_Type(AminoAcidLiteral)
			ReflectMe(AminoAcidLiteral, AST_Node);

			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

			std::string Literal;
		};


		//__________________________________________________________________________________________________
		// Typedefs
		//__________________________________________________________________________________________________
		class GILAPI TypedefNode : public ProjectNode
		{
		public:
			TypedefNode(std::string&& Name) : Name(Name) {}
			TypedefNode(std::string&& Name, std::string Inherits) : Name(Name), Inherits(Inherits) {}

			NoCompile
			AST_Type(TypedefNode)
			virtual void AddToProject(Parser::Project* Proj) override;

			std::string Name;
			std::string Inherits = "any";
		};

		class GILAPI InheritTypedef : public ProjectNode
		{
		public:
			InheritTypedef(std::string&& Type, std::string&& Parent) : Type(Type), Parent(Parent) {}

			NoCompile
			AST_Type(InheritTypedef)
			virtual void AddToProject(Parser::Project* Proj) override;

			std::string Type;
			std::string Parent;
		};


		//Comment class  (doesn't do anything)
		class GILAPI Comment : public AST_Node
		{
			NoCompile
			AST_Type(Comment)
		};



		//For calling params passed to sequence
		class GILAPI UseParam : public AST_Node
		{
		public:
			UseParam() {}
			UseParam(std::string&& ParamName, Call_Params&& Params) : ParamName(ParamName), Params(Params) {}
			
			virtual void Compile(Compiler::CompilerContext& context, Parser::Project* Project) override;
			
			AST_Type(UseParam)
			ReflectMe(UseParam, AST_Node);
			
			virtual void Save(std::ofstream& OutputFile) override;
			virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;
			
			std::string ParamName;
			Call_Params Params;
			
			//The parameter to be compiled
			Param* m_Param;
		};

		//For creating namespaces
		class GILAPI Namespace : public ProjectNode
		{
		public:
			Namespace() {}
			Namespace(std::string&& Name, std::vector<AST_Node*>&& Body) : Name(Name), Body(Body) {}
			
			NoCompile
			AST_Type(Namespace)
			virtual void AddToProject(Parser::Project* Proj) override;

			std::string Name;
			std::vector<AST_Node*> Body;
		};
	}
}