#ifndef PISCENECREATE
#define PISCENECREATE

#include<string>
#include<algorithm>
#include<iostream>
using namespace std;

#include"ieParamFormat.h"
#include"vcConstants.h"
#include"MatDS.h"
using namespace MatDS;
// function scene = piSceneCreate(photons,varargin)
// p.addRequired('photons',@isnumeric);
// p.addParameter('fov',40,@isscalar)               % Horizontal fov, degrees
// p.addParameter('meanluminance',100,@isscalar);
// p.addParameter('wavelength', 400:10:700, @isvector);

Scene piSceneCreate(Photons photons_in, double fov = 40,double meanluminace = 100, Wave wavelength = (Wave)"400:10:700");
#endif