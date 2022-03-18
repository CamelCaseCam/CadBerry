#include <cdbpch.h>
#include "AtomGraph.h"

namespace CDB
{
	int Atom::GetOxidationState()
	{
		if (OxidationState == UninitializedCharge)
		{
			int AtomCharge = CalculateFormalCharge();
			int BondOrder = 0;
			for (Bond* bond : this->Bonds)
			{
				if (bond == nullptr)    //If there's no bond
					continue;
				if (bond->Atom1->m_AtomType == bond->Atom2->m_AtomType)    //Do nothing if they're the same atom
					continue;

				if (bond->Atom1 == this)    //This atom is Atom1
				{
					if (bond->Atom1->GetElectronegativity() > bond->Atom2->GetElectronegativity())    //Assign the electron to the more electronegative atom
						--BondOrder;
					else
						++BondOrder;
				}
				else
				{
					if (bond->Atom1->GetElectronegativity() < bond->Atom2->GetElectronegativity())    //Assign the electron to the more electronegative atom
						--BondOrder;
					else
						++BondOrder;

				}
			}
			OxidationState = AtomCharge + BondOrder;
		}
		return OxidationState;
	}

	int Atom::CalculateFormalCharge()
	{
		if (FormalCharge == UninitializedCharge)
		{
			int ValenceElectrons = GetValenceElectrons();
			int BondedElectrons = GetNumBondedElectrons();
			int UnbondedValenceElectrons = ValenceElectrons - BondedElectrons;
			FormalCharge = ValenceElectrons - UnbondedValenceElectrons - (BondedElectrons / 2);
		}
		return FormalCharge;
	}

	int Atom::GetNumBondedElectrons()
	{
		if (this->BondedElectrons == -1)
		{
			this->BondedElectrons = 0;
			for (Bond* bond : this->Bonds)
			{
				if (bond != nullptr)
					++BondedElectrons;
			}
		}
		return this->BondedElectrons;
	}

	float GetLennardJones(float Dist)
	{
		return 0.0f;
	}
}