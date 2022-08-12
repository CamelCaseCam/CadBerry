#pragma once
#include <cdbpch.h>

#include "CadBerry/Core.h"

namespace CDB
{
	namespace Math
	{
		namespace MathContext    //No need to use a static class for this
		{
			//Because the equations affect the variables on the next steps, we swap which map this pointer points to
			CDBAPI extern std::unordered_map<std::string, float>* CurrentVariables;
			CDBAPI extern std::unordered_map<std::string, float>* NextTimestepVariables;

			CDBAPI extern std::unordered_map<std::string, float> Variables1;
			CDBAPI extern std::unordered_map<std::string, float> Variables2;

			CDBAPI void BeginTimestep(float dt);
			CDBAPI void Init();
		}

		class CDBAPI Value    //Base class for terms, numbers, and variables
		{
		public:
			virtual float Get() = 0;
		};

		enum class Operation
		{
			Add, Sub, Mul, Div, Exp, Root, LThan, GThan, IsEq, None
		};

		class Number : public Value
		{
		public:
			Number(float value) : Value(value) {}
			virtual float Get() override { return Value; }

		private:
			float Value;
		};

		class Term : public Value
		{
		public:
			Term() {}
			Term(Value* LValue, Operation op, Value* RValue) : LValue(LValue), op(op), RValue(RValue) {}
			virtual float Get() override;

		private:
			Value* LValue = nullptr;
			Operation op = Operation::None;
			Value* RValue = nullptr;
		};

		class Variable : public Value
		{
		public:
			Variable(std::string name) : Name(name) {}
			virtual float Get() override;

		private:
			std::string Name;
		};

		class CDBAPI Equation    //Equation sets var to the value of m_Equation each step
		{
		public:
			Equation() {}
			Equation(std::string src);
			Equation(std::string var, Term eq) : var(var), m_Equation(eq) {}

			void Calculate();
		private:
			std::string var;
			Term m_Equation;
		};
	}
}