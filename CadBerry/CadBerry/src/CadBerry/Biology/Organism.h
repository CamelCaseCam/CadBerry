#pragma once

namespace CDB
{
	namespace Biology
	{
		struct CodonUsage
		{
			std::string Codon;
			float fraction;

			static CodonUsage empty;
		};

		class CodonGroup    //Wrapper around vector of codon usage(s). Supports non-cannonical amino acids
		{
		public:
			std::vector<CodonUsage> Codons;

			const inline int GetCount() const { return Codons.size(); }

			inline void ReserveCodons(int count) { Codons.reserve(count); }
			void AddCodon(CodonUsage newCodon);

			inline CodonUsage& operator[](int idx) { return Codons[idx]; }
			CodonUsage& operator[](float fraction);
		};

		class CodonTable
		{
		public:
#ifdef CodonTable_UseUnhashedMap
			std::map<char, CodonGroup> Table;
#else 
			std::unordered_map<char, CodonGroup> Table;
#endif // UseNormalMap
			void AddAminoAcid(char code, CodonGroup Codons) { Table[code] = Codons; }
			void AddAminoAcids(std::initializer_list<std::pair<const char, CodonGroup>> Acids) { Table.insert(Acids); }

			CodonGroup& operator[](char ID) { return Table.at(ID); }
			CodonUsage& GetCodon(char ID, float Fraction);

			char Codon2ID(std::string Codon);
		};

		class OrganismParams
		{
		public:
			CodonTable Codons;

			//Info flags
			bool IsEukaryotic;
		};
	}
}