#pragma once
#include <gilpch.h>

#include "Core.h"

namespace GIL
{
	namespace Compiler
	{
		class GILAPI CodonEncoding
		{
		public:
			CodonEncoding(std::string Organism);
			
			std::string* GetFromLetter(char l, int idx = 0);
			std::pair<char, int> CodonToLetter(std::string codon);
			static char CodonToLetterOnly(std::string codon);

			~CodonEncoding();

			std::string ToString() const;
		private:
			std::unordered_map<char, std::vector<std::string*>*> Code2Codon;
		};

		GILAPI extern std::unordered_map<std::string, char> Codon2Letter;

		inline std::ostream& operator<<(std::ostream& os, const CodonEncoding& ce)
		{
			return os << ce.ToString();
		}
	}
}