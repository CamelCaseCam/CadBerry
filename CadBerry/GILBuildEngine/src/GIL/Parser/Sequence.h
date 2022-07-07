#pragma once
#include <cdbpch.h>
#include "Region.h"
#include "GIL/Lexer/Token.h"

#include "GIL/Types/Type.h"

#include "CadBerry/Utils/memory.h"


namespace GIL
{
	//High, but hopefully low enough to prevent stack overflow
	#define MAX_SEQUENCE_CALL_DEPTH 100

	//Only expose this within GIL
#ifdef GIL_BUILD_DLL
	extern int CurrentSequenceCallDepth;
#endif


#define GetInnerCode(Params) ((GIL::InnerCode*)Params["$InnerCode"].Seq)->m_InnerCode

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


	extern CDBAPI std::vector<GIL::Lexer::Token*> EmptyTokens;
	class Sequence    //Base sequence class that is inherited by different sequence types
	{
	public:
		virtual ~Sequence() {}
		virtual std::pair<std::vector<Parser::Region>, std::string> Get(Parser::Project* Proj, std::map<std::string, Param>& Params) = 0;

		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) = 0;

		virtual std::vector<GIL::Lexer::Token*>& GetTokens() { return EmptyTokens; }

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
	};

	class StaticSequence : public Sequence    //So far we only have static sequences
	{
	public:
		StaticSequence() {}
		StaticSequence(std::vector<GIL::Lexer::Token*> tokens) { this->Tokens = tokens; }
		virtual ~StaticSequence() override;

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(Parser::Project* Proj, std::map<std::string, Param>& Params) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		inline void SetTokens(std::vector<GIL::Lexer::Token*> tokens) { this->Tokens = tokens; IsCompiled = false; }

		std::vector<GIL::Lexer::Token*>& GetTokens() override { return Tokens; }
		
	private:
		std::vector<GIL::Lexer::Token*> Tokens;

		bool IsCompiled = false;
		std::vector<CachedSequenceChunk> SequenceCache;
	};

	class SequenceForward : public Sequence
	{
	public:
		SequenceForward() { DestinationSequence = nullptr; }
		SequenceForward(Sequence* destination, std::string& destinationName) : DestinationSequence(destination), DestinationName(destinationName) {
			this->SeqType = destination->SeqType;
		}

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(Parser::Project* Proj, std::map<std::string, Param>& Params) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		std::vector<GIL::Lexer::Token*>& GetTokens() override;

		Sequence* DestinationSequence;
		std::string& DestinationName = Empty;
	};

	class InnerCode : public Sequence
	{
	public:
		InnerCode(std::pair<std::vector<Parser::Region>, std::string> inner) { this->m_InnerCode = inner; }

		virtual std::pair<std::vector<Parser::Region>, std::string> Get(Parser::Project* Proj, std::map<std::string, Param>& Params) override { return this->m_InnerCode; }

		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override {}

		std::pair<std::vector<Parser::Region>, std::string> m_InnerCode;
	};

	class Operator;
	class Operator : public SequenceForward
	{
	public:
		Operator() {}
		Operator(Sequence* destination, std::string& destinationName, Operator* alternateImplementation) : SequenceForward(destination, destinationName), 
			AlternateImplementation(alternateImplementation) {}
		
		virtual std::pair<std::vector<Parser::Region>, std::string> Get(Parser::Project* Proj, std::map<std::string, Param>& Params) override;

		virtual void Save(std::ofstream& OutputFile) override;
		virtual void Load(std::ifstream& InputFile, Parser::Project* Proj) override;

		Operator* GetLastImplementation();

		//For overrides, allows us to call the old implementation if the types don't match
		CDB::scoped_ptr<Operator> AlternateImplementation;

		//Operators are global, but need to remember where they came from
		Parser::Project* Origin = nullptr;
	};
}