#ifndef EXR2SCENE
#define EXR2SCENE

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include"MatDS.h"
using namespace MatDS;

#include"Exr2Scene.h"
#include"ieParamFormat.h"
#include "piReadEXR.h"
#include "Energy2Quanta.h"
#include "piSceneCreate.h"
#include "find_fov.h"
// function photons = Energy2Quanta(wavelength,energy)
// % Convert energy (watts) to number of photons.
// p.addRequired('inputFile',@(x)(exist(x,'file')));
// %p.addParameter('label','radiance',@(x)(ischar(x)||iscell(x)));
// p.addParameter('pbrt_path',[],@(x)(ischar(x)));
// p.addParameter('wave', 400:10:700, @isnumeric);
Scene Exr2Scene(string inputFile,string pbrt_path = "",Wave wave =(Wave) "400:10:700");

#endif