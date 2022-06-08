#include "cdbpch.h"
#include "Organism.h"

namespace CDB
{
	namespace Biology
	{
		CodonUsage CodonUsage::empty = { "", -1 };

		void CodonGroup::AddCodon(CodonUsage newCodon)
		{
			//If the codon table is empty, just add the new codon
			if (this->Codons.size() == 0)
			{
				this->Codons.push_back(newCodon);
				return;
			}

			//Add the codon so they're in order of usage
			for (int i = 0; i < Codons.size(); ++i)
			{
				if (newCodon.fraction > Codons[i].fraction)
				{
					Codons.emplace(Codons.begin() + i, newCodon);
					return;
				}
			}
			Codons.push_back(newCodon);    //If it was less than all of them, add it to the back
		}

		CodonUsage& CodonGroup::operator[](float fraction)
		{
			if (this->Codons.size() == 0)
				return CodonUsage::empty;
			//Because the codons are ordered by fraction, if the first one has a lower fraction than the target, return it
			if (this->Codons[0].fraction <= fraction)
				return Codons[0];

			//Otherwise, loop until you reach a codon that's further away than the last one
			float distance = 999999999999999.0f;    //You shouldn't have fractions greater than 1, but this should prevent errors if you do
			for (int i = 0; i < Codons.size(); ++i)
			{
				if (Codons[i].fraction == fraction)
					return Codons[i];
				float LocalDistance = abs(Codons[i].fraction - fraction);
				if (LocalDistance > distance)    //If the distance between this codon's fraction is greater than the distance between the last codon's fraction
					return Codons[i - 1];
				distance = LocalDistance;
			}
			return Codons[Codons.size() - 1];    //Return the last codon if the fraction is lower than any actual codon fractions
		}



		CodonUsage& CodonTable::GetCodon(char ID, float Fraction)
		{
			if (this->Table.count(ID))
			{
				return this->Table.at(ID)[Fraction];
			}
			return CodonUsage::empty;
		}

		char CodonTable::Codon2ID(std::string Codon)
		{
			for (std::pair<char, CodonGroup> Amino : this->Table)
			{
				for (CodonUsage CU : Amino.second.Codons)
				{
					if (CU.Codon == Codon)
						return Amino.first;
				}
			}
			return 0;
		}
	}
}