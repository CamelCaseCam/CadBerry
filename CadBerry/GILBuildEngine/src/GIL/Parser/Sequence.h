#pragma once
#include <cdbpch.h>
#include "Region.h"
#include "GIL/Lexer/Token.h"

#include "GIL/Types/Type.h"

#include "CadBerry/Utils/memory.h"

#include "GIL/Parser/AST_Node.h"

#include "GIL/Bools/Bool.h"


namespace GIL
{
	//High, but hopefully low enough to prevent stack overflow
	constexpr unsigned int MAX_SEQUENCE_CALL_DEPTH = 100;

	//Only expose this within GIL
#ifdef GIL_BUILD_DLL
	extern int CurrentSequenceCallDepth;
#endif


#define GetInnerCode(Params) ((GIL::InnerCode*)Params["InnerCode"].Seq)->m_InnerCode

	extern std::string Empty;

	namespace Parser
	{
		class Project;
	}

	struct CachedSequenceChunk
	{
		std::vector<Parser::Region> Regions;
		std::string Code;
		int SeqIdx;
	};

	class Sequence;
	struct Param
	{
		Param() : Seq(nullptr), SourceProj(nullptr) {}
		Param(Sequence* seq, Parser::Project* sourceProj) : Seq(seq), SourceProj(sourceProj) {}
		Param(Sequence* seq, Parser::Project* sourceProj, Type* seqType) : Seq(seq), SourceProj(sourceProj), type(seqType) {}

		Sequence* Seq;
		Parser::Project* SourceProj;
		Type* type = &Type::any;
	};


#define NoSave virtual void Save(std::ofstream& OutputFile) override {} \
virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override {}

#define NoImpl virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override {}

	extern CDBAPI std::vector<GIL::Lexer::Token*> EmptyTokens;
	class Sequence    //Base sequence class that is inherited by different sequence types
	{
	public:
		virtual ~Sequence() {}
		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) = 0;
		void Get(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context);

		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) = 0;

		//For data sequences
		virtual void* Data(Parser::Project* Proj) { return nullptr; }
		virtual void DelData(void* data) { }

		virtual std::vector<GIL::Lexer::Token*>& GetTokens() { return EmptyTokens; }

		//For stuff like forwards
		virtual void CompileTimeInit(Parser::Project* Proj) {}

		//Not making this virtual because I don't see the point
		bool TypesMatch(std::map<std::string, Param>& Params);

		static Sequence* LoadSequence(std::ifstream& InputFile, Parser::Project* Proj);

		std::vector<std::string> ParamIdx2Name;
		std::map<std::string, Type*> ParameterTypes;

		inline Type* GetParameterType(std::string type)
		{
			Type* t = ParameterTypes[type];
			if (t == nullptr)
				t = &Type::any;
			return t;
		}

		Type* SeqType = &Type::any;

		std::vector<std::string> ActiveDistributions;
	protected:
		void SaveBaseSequence(std::ofstream& OutputFile);
		void LoadBaseSequence(std::ifstream& InputFile, Parser::Project* Proj);
	};

	////Deprecated, DO NOT USE
	//class StaticSequence : public Sequence    //So far we only have static sequences
	//{
	//public:
	//	StaticSequence() {}
	//	StaticSequence(std::vector<GIL::Lexer::Token*> tokens) { this->Tokens = tokens; }
	//	virtual ~StaticSequence() override;

	//	virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override;

	//	virtual void Save(std::ofstream& OutputFile) override;
	//	virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

	//	inline void SetTokens(std::vector<GIL::Lexer::Token*> tokens) { this->Tokens = tokens; IsCompiled = false; }

	//	std::vector<GIL::Lexer::Token*>& GetTokens() override { return Tokens; }
	//	
	//private:
	//	std::vector<GIL::Lexer::Token*> Tokens;

	//	bool IsCompiled = false;
	//	std::vector<CachedSequenceChunk> SequenceCache;
	//};

	class DynamicSequence : public Sequence    //So far we only have static sequences
	{
	public:
		DynamicSequence() {}
		DynamicSequence(std::vector<Parser::AST_Node*> Nodes) { this->Nodes = Nodes; }
		virtual ~DynamicSequence() override;

		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		std::vector<Parser::AST_Node*> Nodes;
	};

	class SequenceForward : public Sequence
	{
	public:
		SequenceForward() { DestinationSequence = { nullptr, nullptr }; }
		SequenceForward(Parser::AccessNamespace&& Location, Sequence* destination, Parser::Project* origin, std::string& destinationName) : Location(Location), DestinationSequence({destination, origin}), DestinationName(destinationName) {
			this->SeqType = destination->SeqType;
		}

		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override;
		
		virtual void CompileTimeInit(Parser::Project* Proj) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		std::vector<GIL::Lexer::Token*>& GetTokens() override;

		virtual void* Data(Parser::Project* Proj) override;

		Parser::AccessNamespace Location;
		std::pair<Sequence*, Parser::Project*> DestinationSequence;
		std::string DestinationName;
	};

	//TODO: maybe rewrite this so it keeps a list of nodes
	class InnerCode : public Sequence
	{
	public:
		InnerCode(std::pair<std::vector<Parser::Region>, std::string> inner) { this->m_InnerCode = inner; }

		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override;

		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override {}

		std::pair<std::vector<Parser::Region>, std::string> m_InnerCode;
	};

	class Operator;
	class Operator : public SequenceForward
	{
	public:
		Operator() {}
		Operator(Sequence* destination, Parser::Project* origin, std::string& destinationName, Operator* alternateImplementation) : SequenceForward({}, destination, origin, destinationName),
			AlternateImplementation(alternateImplementation), Origin(origin) {}
		
		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		Operator* GetLastImplementation();

		//For overrides, allows us to call the old implementation if the types don't match
		CDB::scoped_ptr<Operator> AlternateImplementation;

		//Operators are global, but need to remember where they came from
		Parser::Project* Origin = nullptr;
	};

	class BoolSequence : public Sequence
	{
	public:
		BoolSequence(Parser::GILBool* m_bool) : m_bool(m_bool) {}

		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override {}

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		Parser::GILBool* m_bool;
	};


	//This sequence subclass is for internal use. It's a quick way to make a sequence that just returns a constant value. It WILL NOT be saved 
	//with a project, and should not be added to the project's sequence map
	class GILAPI InlineSequence : public Sequence
	{
	public:
		InlineSequence(std::string&& OutText, std::vector<Parser::Region>&& OutRegions) : OutText(OutText), OutRegions(OutRegions) {}
		InlineSequence(std::string& OutText, std::vector<Parser::Region>& OutRegions) : OutText(OutText), OutRegions(OutRegions) {}

		virtual void Get_impl(Parser::Project* Proj, std::map<std::string, Param>& Params, Compiler::CompilerContext& context) override 
		{
			*context.OutputString += OutText;
			context.OutputRegions->reserve(context.OutputRegions->size() + OutRegions.size());
			for (int i = 0; i < OutRegions.size(); ++i)
				context.OutputRegions->push_back(OutRegions[i]);
		}

		NoSave

		std::string OutText;
		std::vector<Parser::Region> OutRegions;
	};

	//DOES NOT own data pointer
	class GILAPI DataSequence : public Sequence
	{
	public:
		DataSequence(void* data) : data(data) {}

		virtual void* Data(Parser::Project* Proj) override { return data; }

		NoSave
		NoImpl

		void* data;
	};
}