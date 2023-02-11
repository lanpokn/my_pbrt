#ifndef PIEXR2MAT
#define PIEXR2MAT

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include<MatDS.h>
using namespace MatDS;

// function data = piEXR2Mat(infile, channelname)
double piExr2Mat(string infile,string channelname);

#endif