#pragma once
#include <gilpch.h>
#include "GIL/Lexer/Token.h"
#include "AST_Node.h"

#include "CadBerry/Utils/memory.h"

namespace GIL
{
namespace Parser
{
	/*
	* This parser uses a generational system of sorts. Instead of starting with higher-level rules and descending like a recursive descent parser,
	* it starts with lower-level rules and builds up more and more complex syntax
	*
	* Rules consist of conditions. If every condition in a rule returns true, the rule adds its output to the parser input/output vector.
	* 
	* Syntax:
	* *condition = condition as many times as possible
	* ~condition = optional condition
	* condition | condition = condition or condition
	* variable % condition = if condition was true, put its node/token in the variable
	* condition < (function returning a void pointer, type name) = if condition was true, add the output of the function to the output
	*/

	void DoNotDelete(void* ptr) { }
	void DeleteToken(void* ptr) { delete (Lexer::Token*)ptr; }
	void DeleteNode(void* ptr) { delete (Parser::AST_Node*)ptr; }

	inline std::string NoType = "NO TYPE";
	inline std::string TokenType = "TOKEN";
	inline std::string NodeType = "NODE";


	//Struct containing a void pointer to the value, used as the input/output to the parser
	struct ParseIO
	{
		void* Val;
		std::function<void(void*)> Del;
		std::string& Type = NoType;
		
		template<typename T>
		ParseIO(T* val, std::function<void(void*)> del, std::string& Type) : Val(val), Del(del), Type(Type) {}
		ParseIO(void* val, std::function<void(void*)> del, std::string& Type) : Val(val), Del(del), Type(Type) {}
		ParseIO() : Val(nullptr) {}

		inline ~ParseIO()
		{
			Del(Val);
		}
	};

	inline std::unordered_map<std::string, ParseIO> TypeName2ParseIO = {
		{"token", {nullptr, DeleteToken, TokenType}},
		{"Token", {nullptr, DeleteToken, TokenType}},
		{"TOKEN", {nullptr, DeleteToken, TokenType}},

		{"node", {nullptr, DeleteNode, NodeType}},
		{"Node", {nullptr, DeleteNode, NodeType}},
		{"NODE", {nullptr, DeleteNode, NodeType}},
	};
	//Interface for conditions
	class Condition
	{
	public:
		virtual ~Condition() {}

		virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) = 0;
		virtual Condition* operator*();
		virtual Condition* operator~();
		virtual Condition* operator|(Condition* other);
	};

	class TokenCondition : public Condition
	{
	public:
		TokenCondition(Lexer::LexerToken TokenType) : TokenType(TokenType) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			if (Nodes[idx].Type != "Token")
			{
				return { false, {} };
			}
			Lexer::Token* tok = (Lexer::Token*)Nodes[idx].Val;

			return { tok->TokenType == this->TokenType, {} };
		}

		Lexer::LexerToken TokenType;
	};

	class NodeCondition : public Condition
	{
	public:
		NodeCondition(std::string&& NodeType) : NodeType(NodeType) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			if (Nodes[idx].Type != "Node")
			{
				return { false, {} };
			}
			Parser::AST_Node* node = (Parser::AST_Node*)Nodes[idx].Val;

			return { node->GetName() == this->NodeType, {} };
		}

		std::string NodeType;
	};

	class MultiCondition : public Condition
	{
	public:
		MultiCondition(Condition* cond) : cond(cond) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			//Loop through the text until you find a node that returns false when evaluated with this condition
			while (idx < Nodes.size() && cond->Evaluate(Nodes, idx).first) ++idx;
			return {true, {}};
		}

		virtual Condition* operator*() override
		{
			return this;
		}

		CDB::scoped_ptr<Condition> cond;
	};


	class OptionalCondition : public Condition
	{
	public:
		OptionalCondition(Condition* cond) : cond(cond) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			//If the child condition returns true, advance
			auto output = cond->Evaluate(Nodes, idx);
			if (output.first) ++idx;
			return output;
		}

		CDB::scoped_ptr<Condition> cond;
	};

	class OrCondition : public Condition
	{
	public:
		OrCondition(Condition* cond1, Condition* cond2) : cond1(cond1), cond2(cond2) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			//If the child condition returns true, advance
			int OldIdx = idx;
			auto output1 = cond1->Evaluate(Nodes, idx);
			if (output1.first) return output1;
			auto output2 = cond2->Evaluate(Nodes, idx);
			idx = OldIdx;
			if (output2.first) return output2;
			return { false, {} };
		}

		CDB::scoped_ptr<Condition> cond1, cond2;
	};


	//________________________________________________________________________________________________
	// Variable stuff
	//________________________________________________________________________________________________


	class Variable
	{
	public:
		Variable() {}

		inline Variable(Lexer::Token* tok) 
		{
			Data.Type = TokenType;
			Data.Val = tok;
			Data.Del = DeleteToken;
		}

		inline Variable(Parser::AST_Node* node)
		{
			Data.Type = NodeType;
			Data.Val = node;
			Data.Del = DeleteNode;
		}

		inline void Set(ParseIO& val)
		{
			//Make sure they're the same type
			if (val.Type != Data.Type)
			{
				CDB_BuildError("Cannot initialize variable of type {0} with ParseIO block of type {1}", Data.Type, val.Type);
				return;
			}
			Data.Del(Data.Val);
			Data.Val = val.Val;
		}

		Condition* operator%(Condition* other);

		ParseIO Data;
	};

	class VariableVector
	{
	public:
		inline VariableVector(std::vector<std::string> types)
		{
			Data.reserve(types.size());
			for (std::string& type : types)
			{
				if (!TypeName2ParseIO.contains(type))
				{
					CDB_BuildError("Error initializing VariableVector: unrecognized type \"{0}\"", type);
					return;
				}
				Data.push_back(TypeName2ParseIO[type]);
			}
		}

		inline VariableVector(std::vector<ParseIO>&& Data) : Data(Data) {}

		inline void Set(std::vector<ParseIO>& data, int begin, int end)
		{
			if (end - begin != Data.size())
			{
				CDB_BuildError("Cannot initialize vector variable of size {0} with vector of size {1}", Data.size(), end - begin);
				return;
			}
			for (int i = begin; i < end && i < data.size(); ++i)
			{
				//Make sure the data types match
				if (data[i].Type != Data[i - begin].Type)
				{
					CDB_BuildError("Cannot initialize variable at index {0} of type {1} with ParseIO block of type {2}", i - begin, Data[i - begin].Type, data[i].Type);
					return;
				}
				Data[i - begin].Del(Data[i - begin].Val);
				Data[i - begin].Val = data[i].Val;
			}
		}

		Condition* operator%(Condition* other);

		std::vector<ParseIO> Data;
	};

	class SetVariable : public Condition
	{
	public:
		SetVariable(Condition* cond, Variable* var) : cond(cond), var(var) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			//if the condition returns true, put the node or token at the index into the variable
			int beginning = idx;
			auto output = cond->Evaluate(Nodes, idx);
			if (output.first)
			{
				var->Set(Nodes[beginning]);
				return output;
			}
			return { false, {} };
		}

		CDB::scoped_ptr<Condition> cond;
		Variable* var;
	};

	class SetVariableVector : public Condition
	{
	public:
		SetVariableVector(Condition* cond, VariableVector* var) : cond(cond), var(var) {}

		inline virtual std::pair<bool, ParseIO> Evaluate(std::vector<ParseIO>& Nodes, int& idx) override
		{
			//if the condition returns true, put the node or token at the index into the variable
			int beginning = idx;
			auto output = cond->Evaluate(Nodes, idx);
			if (output.first)
			{
				var->Set(Nodes, beginning, idx + 1);
				return output;
			}
			return { false, {} };
		}

		CDB::scoped_ptr<Condition> cond;
		VariableVector* var;
	};


	//________________________________________________________________________________________________
	// Operators
	//________________________________________________________________________________________________


	inline Condition* Condition::operator*()
	{
		return new MultiCondition(this);
	}

	inline Condition* Condition::operator~()
	{
		return new OptionalCondition(this);
	}

	inline Condition* Condition::operator|(Condition* other)
	{
		return new OrCondition(this, other);
	}

	inline Condition* Variable::operator%(Condition* other)
	{
		return new SetVariable(other, this);
	}

	inline Condition* VariableVector::operator%(Condition* other)
	{
		return new SetVariableVector(other, this);
	}


} // namespace parser
} // namespace GIL