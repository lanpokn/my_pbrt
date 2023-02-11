#ifndef PIREADEXR
#define PIREADEXR

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include<MatDS.h>
using namespace MatDS;

// parser.addRequired('filename', @ischar);
// parser.addParameter('datatype', 'radiance', @ischar);
double piReadEXR(string filename, string datatype = "radiance");

#endif