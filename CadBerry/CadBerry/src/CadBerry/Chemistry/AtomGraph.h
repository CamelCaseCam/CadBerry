#pragma once
#include <cdbpch.h>

namespace CDB
{
#define UninitializedCharge -999999    //Arbitrary number that isn't phisically possible

	enum class AtomType : unsigned char
	{
		H = 0, He = 1, 
		Li = 2, Be = 3, B = 4, C = 5, N = 6, O = 7, F = 8, Ne = 9, 
		Na = 10, Mg = 11, Al = 12, Si = 13, P = 14, S = 15, Cl = 16, Ar = 17, 
		K = 18, Ca = 19, Sc = 20, Ti = 21, V = 22, Cr = 23, Mn = 24, Fe = 25, Co = 26, Ni = 27, Cu = 28, Zn = 29, Ga = 30, Ge = 31, As = 32, Se = 33, Br = 34, Kr = 35,
		Rb = 36, Sr = 37, Y = 38, Zr = 39, Nb = 40, Mo = 41, Tc = 42, Ru = 43, Rh = 44, Pd = 45, Ag = 46, Cd = 47, In = 48, Sn = 49, Sb = 50, Te = 51, I = 52, Xe = 53, 

		Ta = 54, W = 55, Re = 56, Os = 57, Ir = 58, Pt = 59, Au = 60, Hg = 61, Tl = 62, Pb = 63, Bi = 64, Po = 65, At = 66, Rn = 67    //Only including some of the later elements
	};

	static const constexpr std::array<float, 68> AtomElectronegativity =    //Using allen scale
	{2.300,																													4.160,    //H, He
	 0.912, 1.576,																		 2.051, 2.544, 3.066, 3.610, 4.193, 4.787,    //Li, Be, B, C, N, O, F, Ne
	 0.869, 1.293,																		 1.613, 1.916, 2.253, 2.589, 2.869, 3.242,    //Na, Mg, Al, Si, P, S, Cl, Ar
	 0.734, 1.034, 1.190, 1.380, 1.530, 1.650, 1.750, 1.800, 1.840, 1.880, 1.850, 1.588, 1.756, 1.994, 2.211, 2.424, 2.685, 2.966,    //K-Kr
	 0.706, 0.963, 1.120, 1.320, 1.410, 1.470, 1.510, 1.540, 1.560, 1.580, 1.870, 1.521, 1.656, 1.824, 1.984, 2.158, 2.359, 2.582,    //Rb-Xe
								 1.340, 1.470, 1.600, 1.650, 1.680, 1.720, 1.920, 1.765, 1.789, 1.854, 2.010, 2.190, 2.390, 2.600};   //Ta-Rn

	static const constexpr std::array<int, 68> AtomValenceElectrons =    //Table for runtime lookup of atom valence electrons
	{1,													  2,
	 1, 2,								   3, 4, 5, 6, 7, 8,
	 1, 2,								   3, 4, 5, 6, 7, 8,
	 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 3, 4, 5, 6, 7, 8,
	 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 3, 4, 5, 6, 7, 8, 
				 4, 5, 6, 7, 8, 9, 10, 11, 3, 4, 5, 6, 7, 8};

	inline float GetAtomElectronegativity(AtomType atomType) { return AtomElectronegativity[(char)atomType]; }

	inline float GetENegDiff(float EN1, float EN2) { return abs(EN1 - EN2); }

	inline float GetLennardJones(float x1, float y1, float z1, float x2, float y2, float z2);
	inline float GetLennardJones(float Dist);

	struct Bond;
	class Atom
	{
	public:
		AtomType m_AtomType;

		double XPos, YPos, ZPos;

		Bond* Bonds[7];    //If bond doesn't exist it will be nullptr

		int GetOxidationState();
		int CalculateFormalCharge();
		int GetNumBondedElectrons();

		inline int GetValenceElectrons() { return AtomValenceElectrons[(char)m_AtomType]; }
		inline float GetElectronegativity() { return AtomElectronegativity[(char)m_AtomType]; }

	private:
		int BondedElectrons = -1;
		int FormalCharge = UninitializedCharge;
		int OxidationState = UninitializedCharge;
	};

	struct Bond
	{
		Atom* Atom1;
		Atom* Atom2;

		float DegreesXY, DegreesZY;
		float BondLength;
	};
}