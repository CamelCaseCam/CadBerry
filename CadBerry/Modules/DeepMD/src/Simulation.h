#pragma once

#include "Atom.h"
#include <vector>

//The main simulation data structure
class Simulation
{
public:
	Simulation() {}

	std::vector<Atom> Atoms;
};