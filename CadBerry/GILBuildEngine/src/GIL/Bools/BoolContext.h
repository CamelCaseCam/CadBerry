#pragma once
#include <gilpch.h>
#include "IfStatement.h"

namespace GIL
{
	enum class BinOpType
	{
		AND,
		OR,
		NOT,
		NAND,
		NOR,
	};

	extern std::unordered_map<BinOpType, std::string> BinOpType2Str;

	namespace BoolContext
	{
		class Value
		{
		public:
			Value(std::string Name) : BoolName(Name) {}
			std::string BoolName;
		};

		class BinOp
		{
		public:
			BinOp(Value* l, BinOpType type, Value* r) : LValue(l), OpType(type), RValue(r) {}

			Value* LValue;
			BinOpType OpType;
			Value* RValue;
		};

		extern std::map<std::string, BinOp*> Bools;
		extern std::vector<IfStatement*> IfStatements;

		extern int IntermediateNum;
		Value* AllocIntermediateBool(BinOp* Op);

		void Print();

		inline std::ostream& operator<<(std::ostream& os, const BinOp& b)
		{
			if (b.OpType == BinOpType::NOT)
			{
				return os << "NOT " << b.LValue;
			}
			os << b.LValue->BoolName;
			os << " ";
			os << BinOpType2Str[b.OpType];
			os << " ";
			os << b.RValue->BoolName;
			return os;
		}
	}
}