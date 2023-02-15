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
//1 - 31通道，没有32通道，datasize
typedef struct channels_data{
    vector<string> allname;
    vector<float*> allData;
    int DataSize;
    int height;
    int width;
    channels_data(){
        DataSize = 0;
        height = 0;
        width = 0;
        allname.clear();
        allData.clear();
    }
    ~channels_data(){
        //手动释放掉这些内存
        int size = allData.size();
        for(int i = 0;i<size;i++){
            delete[] allData.at(i);
            allData.at(i)= NULL;
        }
        allname.clear();
        allData.clear();
    }
    // //复制构造函数
    // channels_data(const struct channels_data & c){
    //     allname = c.allname;
    //     DataSize = c.DataSize;
    //     height = c.height;
    //     width = c.width;
    //     for(int j =0;j<c.allData.size();j++){
    //         float *buf_exr = new float[c.DataSize];
    //         for(int k=0;k<c.DataSize;k++){
    //             buf_exr[k] = c.allData.at(j)[k];
    //         }
    //     }
    // }
}allCHA;

void MyImgTool(string inFile,string channnelName,allCHA& ret);

#endif