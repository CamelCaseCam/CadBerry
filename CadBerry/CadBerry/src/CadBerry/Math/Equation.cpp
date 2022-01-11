#include <cdbpch.h>
#include "Equation.h"
#include "CadBerry/Log.h"

namespace CDB
{
	namespace Math
	{
		namespace MathContext    //No need to use a static class for this
		{
			//Because the equations affect the variables on the next steps, we swap which map this pointer points to
			std::unordered_map<std::string, float>* CurrentVariables;
			std::unordered_map<std::string, float>* NextTimestepVariables;

			std::unordered_map<std::string, float> Variables1;
			std::unordered_map<std::string, float> Variables2;

			void BeginTimestep(float dt)
			{
				//Swap the variable maps
				std::unordered_map<std::string, float>* OldVar = CurrentVariables;
				std::unordered_map<std::string, float>* NewVar = NextTimestepVariables;
				
				CurrentVariables = NewVar;
				NextTimestepVariables = OldVar;

				//Set the delta time
				(*CurrentVariables)["dt"] = dt;
			}
			void Init()
			{
				CurrentVariables = &Variables1;
				NextTimestepVariables = &Variables2;
				Variables1["dt"] = 0.0f;
			}
		}

		float Term::Get()
		{
			switch (op)
			{
			case CDB::Math::Operation::Add:
				return LValue->Get() + RValue->Get();
			case CDB::Math::Operation::Sub:
				return LValue->Get() - RValue->Get();
			case CDB::Math::Operation::Mul:
				return LValue->Get() * RValue->Get();
			case CDB::Math::Operation::Div:
				return LValue->Get() / RValue->Get();
			case CDB::Math::Operation::Exp:
				return std::powf(LValue->Get(), RValue->Get());
			case CDB::Math::Operation::Root:
				return std::powf(LValue->Get(), 1.0/RValue->Get());
			case CDB::Math::Operation::GThan:
				return (float)(LValue->Get() > RValue->Get());
			case CDB::Math::Operation::LThan:
				return (float)(LValue->Get() < RValue->Get());
			case CDB::Math::Operation::IsEq:
				return (float)(LValue->Get() == RValue->Get());
			default:
				return 0.0f;
			}
		}

		float Variable::Get()
		{
			return (*MathContext::CurrentVariables)[this->Name];
		}

		void Equation::Calculate()
		{
			(*MathContext::NextTimestepVariables)[this->var] = this->m_Equation.Get();
		}

		enum class TokenType
		{
			Num, Var, Add, Sub, Mul, Div, Exp, Root, LParen, RParen, GThan, LThan, IsEq
		};

		struct Token
		{
			Token() {}
			Token(TokenType tt, std::string value) : m_TokenType(tt), Value(value) {}

			TokenType m_TokenType;
			std::string Value;
		};

		std::vector<Token> Tokenize(std::string& src, int start);
		Value* Parse(std::vector<Token>& Tokens, Operation OpType, int& idx);
		Term ParseEquation(std::vector<Token>& Tokens, int& idx, int ReturnLevel = 0);
		int GetOperatorPrecendence(Operation op);

		Equation::Equation(std::string src)
		{
			int EqIdx = src.find("=");
			CDB_EditorInfo(EqIdx);
			if (EqIdx == -1)
			{
				CDB_BuildError("Expected variable assignment in equation");
				return;
			}
			if (src[EqIdx - 1] == ' ')    //The last char of a variable shouldn't be a space
			{
				this->var = src.substr(0, EqIdx - 1);
			}
			else
			{
				this->var = src.substr(0, EqIdx);
			}

			std::vector<Token> Tokens = Tokenize(src, EqIdx + 1);
			int idx = 0;
			this->m_Equation = ParseEquation(Tokens, idx);
		}

#define IsNumber case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0'
#define InAlphabet case 'a':\
case 'b':\
case 'c':\
case 'd':\
case 'e':\
case 'f':\
case 'g':\
case 'h':\
case 'i':\
case 'j':\
case 'k':\
case 'l':\
case 'm':\
case 'n':\
case 'o':\
case 'p':\
case 'q':\
case 'r':\
case 's':\
case 't':\
case 'u':\
case 'v':\
case 'w':\
case 'x':\
case 'y':\
case 'z':\
case '_'

		Token GetNumber(const std::string& src, int& i);
		Token GetWord(const std::string& src, int& i);

		std::vector<Token> Tokenize(std::string& src, int start)
		{
			std::vector<Token> Output;

			bool WasOp = false;
			for (int i = start; i < src.length(); ++i)
			{
				switch (std::tolower(src[i]))
				{
				IsNumber:
				Number:
					WasOp = false;
					Output.push_back(GetNumber(src, i));
					break;

				//Operations
				case '+':
					WasOp = true;
					Output.push_back(Token(TokenType::Add, ""));
					break;
				case '-':
					if (WasOp)
						goto Number;
					WasOp = true;
					Output.push_back(Token(TokenType::Sub, ""));
					break;
				case '*':
					WasOp = true;
					Output.push_back(Token(TokenType::Mul, ""));
					break;
				case '/':
					WasOp = true;
					Output.push_back(Token(TokenType::Div, ""));
					break;
				case '^':
					WasOp = true;
					Output.push_back(Token(TokenType::Exp, ""));
					break;
				case '<':
					WasOp = true;
					Output.push_back(Token(TokenType::LThan, ""));
					break;
				case '>':
					WasOp = true;
					Output.push_back(Token(TokenType::GThan, ""));
					break;
				case '=':
					WasOp = true;
					Output.push_back(Token(TokenType::IsEq, ""));
					break;
				
				InAlphabet:
					WasOp = false;
					Output.push_back(GetWord(src, i));
					break;

				case '(':
					Output.push_back(Token(TokenType::LParen, ""));
					break;
				case ')':
					Output.push_back(Token(TokenType::RParen, ""));
					break;
				default:
					break;
				}
			}

			return std::move(Output);    //Preventing RVO, because otherwise the function places the loop after the return statement
		}

		Token GetNumber(const std::string& src, int& i)
		{
			Token output;
			output.m_TokenType = TokenType::Num;
			output.Value += src[i];
			++i;

			for (i; i < src.length(); ++i)
			{
				switch (src[i])
				{
				IsNumber:
				case '.':
					output.Value += src[i];
					break;
				default:
					--i;
					return output;
				}
			}
			return output;
		}

		Token GetWord(const std::string& src, int& i)
		{
			Token Output;
			for (i; i < src.length(); ++i)
			{
				switch (std::tolower(src[i]))
				{
				IsNumber:
				InAlphabet:
					Output.Value += src[i];
					break;
				default:
					--i;
					Output.m_TokenType = TokenType::Var;
					if (Output.Value == "rt")
						Output.m_TokenType = TokenType::Root;
					return std::move(Output);    //RVO being annoying again
				}
			}

			Output.m_TokenType = TokenType::Var;
			if (Output.Value == "rt")
				Output.m_TokenType = TokenType::Root;
			return std::move(Output);    //RVO being annoying again
		}

		Term ParseEquation(std::vector<Token>& Tokens, int& idx, int ReturnLevel)
		{
			Value* LValue;
			switch (Tokens[idx].m_TokenType)
			{
			case TokenType::Num:
				LValue = new Number(std::stof(Tokens[idx].Value));
				break;
			case TokenType::Var:
				LValue = new Variable(Tokens[idx].Value);
				break;
			case TokenType::LParen:
			{
				++idx;
				Term* t = new Term(std::move(ParseEquation(Tokens, idx)));
				++idx;
				if (idx == Tokens.size() - 1 || Tokens[idx + 1].m_TokenType == TokenType::RParen)
					return *t;
				LValue = t;
				break;
			}
			
			default:
				//There's no way we can salvage this without memory exceptions later on, so we exit the program
				CDB_EditorFatal("Illegal LValue token type");
			}

			//Now we have an LValue
			++idx;
			Operation OpType;
			switch (Tokens[idx].m_TokenType)
			{
			case TokenType::Add:
				OpType = Operation::Add;
				break;
			case TokenType::Sub:
				OpType = Operation::Sub;
				break;
			case TokenType::Mul:
				OpType = Operation::Mul;
				break;
			case TokenType::Div:
				OpType = Operation::Div;
				break;
			case TokenType::Exp:
				OpType = Operation::Exp;
				break;
			case TokenType::Root:
				OpType = Operation::Root;
				break;
			case TokenType::LThan:
				OpType = Operation::LThan;
				break;
			case TokenType::GThan:
				OpType = Operation::GThan;
				break;
			case TokenType::IsEq:
				OpType = Operation::IsEq;
				break;
			}

			Value* RValue;

			GetRValue:
			++idx;
			RValue = Parse(Tokens, OpType, idx);

			//Now, check if there's more in the equation
			if (idx == Tokens.size() - 1)
				return { LValue, OpType, RValue };
			//Check for RParen
			if (Tokens[idx + 1].m_TokenType == TokenType::RParen)
				return { LValue, OpType, RValue };

			Operation NextOp;
			switch (Tokens[idx + 1].m_TokenType)
			{
			case TokenType::Add:
				NextOp = Operation::Add;
				break;
			case TokenType::Sub:
				NextOp = Operation::Sub;
				break;
			case TokenType::Mul:
				NextOp = Operation::Mul;
				break;
			case TokenType::Div:
				NextOp = Operation::Div;
				break;
			case TokenType::Exp:
				NextOp = Operation::Exp;
				break;
			case TokenType::Root:
				NextOp = Operation::Root;
				break;
			case TokenType::LThan:
				NextOp = Operation::LThan;
				break;
			case TokenType::GThan:
				NextOp = Operation::GThan;
				break;
			case TokenType::IsEq:
				NextOp = Operation::IsEq;
				break;
			}
			if (ReturnLevel >= GetOperatorPrecendence(NextOp))
				return { LValue, OpType, RValue };
			++idx;
			LValue = new Term(LValue, OpType, RValue);
			OpType = NextOp;
			goto GetRValue;
		}

		int GetOperatorPrecendence(Operation op)
		{
			switch (op)
			{
			case Operation::LThan:
			case Operation::GThan:
			case Operation::IsEq:
			case Operation::Sub:
			case Operation::Add:
				return 1;
			case Operation::Mul:
			case Operation::Div:
				return 2;
			case Operation::Exp:
			case Operation::Root:
				return 3;
			}
		}

		Value* Parse(std::vector<Token>& Tokens, Operation OpType, int& idx)
		{
			if (idx == Tokens.size() - 1 || (idx == Tokens.size() - 2 && Tokens[idx + 1].m_TokenType == TokenType::RParen))    //This is the last RValue
			{
				switch (Tokens[idx].m_TokenType)
				{
				case TokenType::Num:
					return new Number(std::stof(Tokens[idx].Value));
				case TokenType::Var:
					return new Variable(Tokens[idx].Value);
				default:
					CDB_BuildError("Unexpected end of equation");
				}
			}

			switch (OpType)
			{
			case Operation::Root:
			case Operation::Exp:
				switch (Tokens[idx].m_TokenType)
				{
				case TokenType::Num:
					return new Number(std::stof(Tokens[idx].Value));
				case TokenType::Var:
					return new Variable(Tokens[idx].Value);
				case TokenType::LParen:
					++idx;
					return new Term(std::move(ParseEquation(Tokens, idx)));
				}
				break;
			case Operation::Mul:
			case Operation::Div:
				//If this isn't the end of the equation or the term
				if (idx != Tokens.size() - 1 && Tokens[idx + 1].m_TokenType != TokenType::RParen)
				{
					return new Term(std::move(ParseEquation(Tokens, idx, 2)));
				}
				else
				{
					switch (Tokens[idx].m_TokenType)
					{
					case TokenType::Num:
						return new Number(std::stof(Tokens[idx].Value));
					case TokenType::Var:
						return new Variable(Tokens[idx].Value);
					}
				}
				break;
			case Operation::GThan:
			case Operation::LThan:
			case Operation::IsEq:
			case Operation::Sub:
			case Operation::Add:
				//If this isn't the end of the equation or the term
				if (idx != Tokens.size() - 1 && Tokens[idx + 1].m_TokenType != TokenType::RParen)
				{
					return new Term(std::move(ParseEquation(Tokens, idx, 2)));
				}
				else
				{
					switch (Tokens[idx].m_TokenType)
					{
					case TokenType::Num:
						return new Number(std::stof(Tokens[idx].Value));
					case TokenType::Var:
						return new Variable(Tokens[idx].Value);
					}
				}
				break;
			}
		}
	}
}