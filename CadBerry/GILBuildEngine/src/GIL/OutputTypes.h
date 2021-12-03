#pragma once
#include <gilpch.h>

namespace GIL
{
	enum class OutputType
	{
		GB,
		CGIL,
		FASTA,
	};

	extern std::unordered_map<std::string, OutputType> String2OutputType;    //Dictionary so that we can use this in a switch statement
}