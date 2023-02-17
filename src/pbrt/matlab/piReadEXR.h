#ifndef PIREADEXR
#define PIREADEXR

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include"MatDS.h"
using namespace MatDS;

// parser.addRequired('filename', @ischar);
// parser.addParameter('datatype', 'radiance', @ischar);
//返回一般是三维的，不是三维就强行11n结构吧
Energy piReadEXR(string filename, string datatype = "radiance");

#endif