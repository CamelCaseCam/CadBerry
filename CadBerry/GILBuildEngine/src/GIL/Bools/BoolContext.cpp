#include <gilpch.h>
#include "BoolContext.h"

namespace GIL
{
	std::unordered_map<BinOpType, std::string> BinOpType2Str = std::unordered_map<BinOpType, std::string>({
		{BinOpType::AND, "AND"},
		{BinOpType::OR, "OR"},
		{BinOpType::NOT, "NOT"},
		{BinOpType::NOR, "NOR"},
		{BinOpType::NAND, "NAND"}
	});

	namespace BoolContext
	{
		std::map<std::string, BinOp*> Bools;
		std::vector<IfStatement*> IfStatements;
		int IntermediateNum = -1;

		Value* AllocIntermediateBool(BinOp* Op)
		{
			IntermediateNum += 1;
			std::string Name = "$Auto" + std::to_string(IntermediateNum);
			Bools[Name] = Op;
			return new Value(Name);
		}
		void Print()
		{
			CDB_BuildInfo("GIL BoolContext summary");
			CDB_BuildTrace("Autos: {0}", IntermediateNum + 1);
			CDB_BuildTrace("_____________________________");

			for (auto b : Bools)
			{
				CDB_BuildTrace("{0} : {1}", b.first, *b.second);
			}
		}
	}
}