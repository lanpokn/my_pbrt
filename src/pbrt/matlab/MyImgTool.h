#ifndef MYIMGTOOL
#define MYIMGTOOL

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
double MyImgTool(string inputFile,string channnelName = "");

#endif