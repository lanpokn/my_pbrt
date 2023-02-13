#ifndef MYIMGTOOL
#define MYIMGTOOL

#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;
#include"MatDS.h"
using namespace MatDS;

#include <pbrt/pbrt.h>

#include <pbrt/filters.h>
#include <pbrt/options.h>
#include <pbrt/util/args.h>
#include <pbrt/util/check.h>
#include <pbrt/util/color.h>
#include <pbrt/util/colorspace.h>
#include <pbrt/util/file.h>
#include <pbrt/util/image.h>
#include <pbrt/util/log.h>
#include <pbrt/util/math.h>
#include <pbrt/util/parallel.h>
#include <pbrt/util/print.h>
#include <pbrt/util/progressreporter.h>
#include <pbrt/util/rng.h>
#include <pbrt/util/sampling.h>
#include <pbrt/util/spectrum.h>
#include <pbrt/util/string.h>
#include <pbrt/util/vecmath.h>

extern "C" {
#include <skymodel/ArHosekSkyModel.h>
}

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

// p = inputParser;
// p.addRequired('inputFile',@(x)(exist(x,'file')));
// %p.addParameter('label','radiance',@(x)(ischar(x)||iscell(x)));
// p.addParameter('pbrt_path',[],@(x)(ischar(x)));
double MyImgTool(string inFile,string channnelName = "");

#endif