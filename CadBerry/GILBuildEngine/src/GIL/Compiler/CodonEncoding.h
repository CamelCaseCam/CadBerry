#pragma once
#include <gilpch.h>

namespace GIL
{
	namespace Compiler
	{
		class CodonEncoding
		{
		public:
			CodonEncoding(std::string Organism);
			
			std::string* GetFromLetter(char l, int idx = 0);
			char CodonToLetter(std::string codon);

			~CodonEncoding();

			std::string ToString() const;
		private:
			std::unordered_map<char, std::vector<std::string*>*> Code2Codon;
		};

		extern std::unordered_map<std::string, char> Codon2Letter;

		inline std::ostream& operator<<(std::ostream& os, const CodonEncoding& ce)
		{
			return os << ce.ToString();
		}
	}
}