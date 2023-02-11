#ifndef ENERGY2QUANTA
#define ENERGY2QUANTA

#include<string>
#include<algorithm>
#include<iostream>
using namespace std;
#include<MatDS.h>
using namespace MatDS;
// function photons = Energy2Quanta(wavelength,energy)
// % Convert energy (watts) to number of photons.
Photons Energy2Quanta(Wave wavelength,Energy energy);

#endif