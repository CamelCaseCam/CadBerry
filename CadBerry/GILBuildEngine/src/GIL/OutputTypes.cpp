#include <gilpch.h>
#include "OutputTypes.h"

namespace GIL
{
	std::unordered_map<std::string, OutputType> String2OutputType({
		{"gb", OutputType::GB},
		{"cgil", OutputType::CGIL},
		{"fasta", OutputType::FASTA},
	});
}