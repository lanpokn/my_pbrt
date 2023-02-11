#ifndef PIEXR2ISET
#define PIEXR2ISET

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include<MatDS.h>
using namespace MatDS;

// p = inputParser;
// p.addRequired('inputFile',@(x)(exist(x,'file')));
// %p.addParameter('label','radiance',@(x)(ischar(x)||iscell(x)));
// p.addParameter('pbrt_path',[],@(x)(ischar(x)));
double Exr2ISET(string inputFile,string pbrt_path = "");

#endif