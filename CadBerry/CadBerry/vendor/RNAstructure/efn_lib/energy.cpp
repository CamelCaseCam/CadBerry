//
// Created by Max Ward on 27 March 2017.
//

#include "energy.hpp"

#include <cmath>

<<<<<<< HEAD
using namespace std;
=======
//
using namespace std;
//
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807

efnmax::energy_t efnmax::KCalToEnergy(efnmax::kcalmol_t kcalsmol) {
	return static_cast<efnmax::energy_t>(round(kcalsmol / efnmax::EnergyIota));
}


efnmax::kcalmol_t efnmax::EnergyToKCal(efnmax::energy_t e) {
	return e * efnmax::EnergyIota;
}