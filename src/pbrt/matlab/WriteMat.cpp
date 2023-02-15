//未来目标：如果将来还想继续做这个，一定要把创建都封装成函数
//前往别忘记最后全释放掉
//重要教训：matlab不能使用c++malloc出来的东西，matlab无法正常读取，必须用mxMalloc，mxFree,但是即使用这个也不行
// double * const data_temp = (double * const)mxMalloc(sizeof(double)*scene.spectrum.wave.wave.size());
//memcopy后边那个参数，表示需要copy多少内存的东西进去，使用数组时就真的是所有的大小，使用指针的话必须指针长度乘以总共才对！
//如果反了就在这里调整，就是在data赋值的时候排列组合即可，千万别去前边
//don't forget Scene to scene
#include </home/lanpokn/Documents/MATLAB/extern/include/mat.h>
#include </home/lanpokn/Documents/MATLAB/extern/include/matrix.h>
#include"MatDS.h"
#include"WriteMat.h"
#include<iostream>
#include<cstring>

#define NUMBER_OF_STRUCTS 1
#define NUMBER_OF_FIELDS (sizeof(field_names) / sizeof(*field_names))

using namespace std;
using namespace MatDS;
int WriteMat(Scene scene){
    //can be load properly

    //不是那么简单的！！要存一个结构体，
    MATFile *pmat;
    std::vector<int> myInts;
    myInts.push_back(1);
    myInts.push_back(2);
    printf("Accessing a STL vector: %d\n", myInts[1]);

    // double data[9] = { 1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0 };
    string FIlename = scene.name+".mat";
    const char *file = FIlename.c_str();
    char str[256];
    int status; 

    printf("Creating file %s...\n\n", file);
    pmat = matOpen(file, "w");
    if (pmat == NULL) {
        printf("Error creating file %s\n", file);
        printf("(Do you have write permission in this directory?)\n");
        return(EXIT_FAILURE);
    }

    mxArray* plhs;
    mwSize dims[2] = {1, NUMBER_OF_STRUCTS};
    const char* field_names[] = {"type", "metadata","spectrum","distance","magnification","data","illuminant","wAngular","depthMap","name"};
    plhs  = mxCreateStructArray(2,dims,NUMBER_OF_FIELDS,field_names);
    int name_field = mxGetFieldNumber(plhs, "name");
    int type_field = mxGetFieldNumber(plhs, "type");
    int spectrum_field = mxGetFieldNumber(plhs, "spectrum");
    int distance_field = mxGetFieldNumber(plhs, "distance");
    int magnification_field = mxGetFieldNumber(plhs, "magnification");
    int data_field = mxGetFieldNumber(plhs, "data");
    int illuminant_field = mxGetFieldNumber(plhs, "illuminant");
    int wAngular_field = mxGetFieldNumber(plhs, "wAngular");
    int depthMap_field = mxGetFieldNumber(plhs, "depthMap");

    //把所有最终的scene,put进去
    //没有那么简单，微信问老师，明天应该就有答案了，总之数组可以，指针不行,use[] first
    mxSetFieldByNumber(plhs, 0, name_field, mxCreateString(scene.name.c_str()));
    mxSetFieldByNumber(plhs, 0, type_field, mxCreateString(scene.type.c_str()));
    mxSetFieldByNumber(plhs, 0, distance_field, mxCreateDoubleScalar(scene.distance));
    mxSetFieldByNumber(plhs, 0, magnification_field, mxCreateDoubleScalar(scene.magnification));
    mxSetFieldByNumber(plhs, 0, wAngular_field, mxCreateDoubleScalar(scene.wAngular));
        //加入spectrum，这在某种意义上是递归加入的
        mxArray *plhs_spec;
        const char* spec_field_names[] = {"wave"};
        int spec_num_of_field =  (sizeof(spec_field_names) / sizeof(*spec_field_names));
        //该情况下dims应该都可以通用
        mwSize dims_spec[2] = {1, 1};
        plhs_spec  = mxCreateStructArray(2,dims_spec,spec_num_of_field,spec_field_names);
        int spec_wave_field = mxGetFieldNumber(plhs_spec, "wave");
            //对wave赋值
            mxArray *plhs_spec_wave;
            plhs_spec_wave = mxCreateDoubleMatrix(1,31,mxREAL);
            //套了这么多主要是为了方便初始化，好像有5点蠢了。。
            double *data_temp = (double*)mxMalloc(sizeof(double)*scene.spectrum.wave.wave.size());
            // double data_temp[31];
            for(int i= 0;i<scene.spectrum.wave.wave.size();i++){
                data_temp[i] = scene.spectrum.wave.wave.at(i);
            }
            if (plhs_spec_wave == NULL) {
                printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
                printf("Unable to create mxArray.\n");
                return(EXIT_FAILURE);
            }
            memcpy((void *)(mxGetPr(plhs_spec_wave)), (void *)data_temp, sizeof(double)*scene.spectrum.wave.wave.size());
            //把数据都存到了temp里，就可以进一步存到wave里了
            mxSetFieldByNumber(plhs_spec, 0, spec_wave_field, plhs_spec_wave);
    mxSetFieldByNumber(plhs, 0, spectrum_field, plhs_spec);
    mxFree(data_temp);


    //add depth
    //注意matlab都是height，width，1这样在c++中十分不方便，我一律存为1，height,width
    //radiance的存储也因此变成了： C0i,height,width,注意最后存的时候能对的上即可
    mxArray *plhs_depth;
    int depth_height =  scene.depthMap.at(0).size();
    int depth_width = scene.depthMap.at(0).at(0).size();
    int depth_size = depth_height*depth_width ;
    double *data_depth = (double*)mxMalloc(sizeof(double)*depth_size);
    for(int x=0;x<depth_width;x++){
        for(int y=0;y<depth_height;y++){
            data_depth[x*depth_height+y] = scene.depthMap.at(0).at(y).at(x);
        }
    }
    plhs_depth = mxCreateDoubleMatrix(depth_height,depth_width,mxREAL);
    memcpy((void *)(mxGetPr(plhs_depth)), (void *)data_depth, sizeof(double)*depth_size);
    mxSetFieldByNumber(plhs, 0, depthMap_field, plhs_depth);
// it is ambigous
//     for (int z = 0; z < 2; ++z) {
//         // MATLAB按列优先存储
//         for (int y = 0; y < 4; ++y) {
//             for (int x = 0; x < 3; ++x) {
//                 (*d++) = src[z * 3 * 4 + y * 4 + x];
//             }
//         }
//     }
    //加入illuminant结构
    mxArray *plhs_illu;
    //加入spectrum，这在某种意义上是递归加入的
    const char* illu_field_names[] = {"name","type","spectrum","data"};
    int illu_num_of_field =  (sizeof(illu_field_names) / sizeof(*illu_field_names));
    //该情况下dims应该都可以通用
    mwSize dims_illu[2] = {1, 1};
    plhs_illu  = mxCreateStructArray(2,dims_illu,illu_num_of_field,illu_field_names);
    //添加name
    int illu_name_field = mxGetFieldNumber(plhs_illu, "name");
    mxSetFieldByNumber(plhs_illu, 0, illu_name_field, mxCreateString(scene.illuminant.name.c_str()));
    //添加type
    int illu_type_field = mxGetFieldNumber(plhs_illu, "type");
    mxSetFieldByNumber(plhs_illu, 0, illu_type_field, mxCreateString(scene.illuminant.type.c_str()));
    //添加spectrum，直接把过去的specturm拿过来用
    int illu_spec_field = mxGetFieldNumber(plhs_illu, "spectrum");
        //加入spectrum，这在某种意义上是递归加入的
        mxArray *plhs_illu_spec;
        const char* illu_spec_field_names[] = {"wave"};
        int illu_spec_num_of_field =  (sizeof(illu_spec_field_names) / sizeof(*illu_spec_field_names));
        //该情况下dims应该都可以通用
        mwSize dims_illu_spec[2] = {1, 1};
        plhs_illu_spec  = mxCreateStructArray(2,dims_illu_spec,illu_spec_num_of_field,illu_spec_field_names);
        int illu_spec_wave_field = mxGetFieldNumber(plhs_illu_spec, "wave");
            //对wave赋值
            mxArray *plhs_illu_spec_wave;
            plhs_illu_spec_wave = mxCreateDoubleMatrix(1,31,mxREAL);
            //套了这么多主要是为了方便初始化，好像有5点蠢了。。
            double *data_illu_spec = (double*)mxMalloc(sizeof(double)*scene.spectrum.wave.wave.size());
            // double data_temp[31];
            for(int i= 0;i<scene.spectrum.wave.wave.size();i++){
                data_illu_spec[i] = scene.spectrum.wave.wave.at(i);
            }
            if (plhs_illu_spec_wave == NULL) {
                printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
                printf("Unable to create mxArray.\n");
                return(EXIT_FAILURE);
            }
            memcpy((void *)(mxGetPr(plhs_illu_spec_wave)), (void *)data_illu_spec, sizeof(double)*scene.spectrum.wave.wave.size());
            //把数据都存到了temp里，就可以进一步存到wave里了
        mxSetFieldByNumber(plhs_illu_spec, 0, illu_spec_wave_field, plhs_illu_spec_wave);
        //把数据都存到了temp里，就可以进一步存到wave里了
        mxFree(data_illu_spec);
    //把做好的spectrum加进去
    mxSetFieldByNumber(plhs_illu, 0, illu_spec_field, plhs_illu_spec);
    //photons应该也也一样，唯一的区别是31x1,值我就不管了
    int illu_data_field = mxGetFieldNumber(plhs_illu, "data");
        //加入data，这在某种意义上是递归加入的
        mxArray *plhs_illu_data;
        const char* illu_data_field_names[] = {"photons"};
        int illu_data_num_of_field =  (sizeof(illu_data_field_names) / sizeof(*illu_data_field_names));
        //该情况下dims应该都可以通用
        mwSize dims_illu_data[2] = {1, 1};
        plhs_illu_data  = mxCreateStructArray(2,dims_illu_data,illu_data_num_of_field,illu_data_field_names);
        int illu_data_photons_field = mxGetFieldNumber(plhs_illu_data, "photons");
            //对photons赋值
            mxArray *plhs_illu_data_photons;
            plhs_illu_data_photons = mxCreateDoubleMatrix(31,1,mxREAL);
            //套了这么多主要是为了方便初始化，好像有5点蠢了。。
            double *data_illu_data = (double*)mxMalloc(sizeof(double)*scene.spectrum.wave.wave.size());
            // double data_temp[31];
            for(int i= 0;i<scene.spectrum.wave.wave.size();i++){
                data_illu_data[i] = scene.spectrum.wave.wave.at(i);
            }
            if (plhs_illu_data_photons == NULL) {
                printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
                printf("Unable to create mxArray.\n");
                return(EXIT_FAILURE);
            }
            memcpy((void *)(mxGetPr(plhs_illu_data_photons)), (void *)data_illu_data, sizeof(double)*scene.spectrum.wave.wave.size());
            //把数据都存到了temp里，就可以进一步存到wave里了
        mxSetFieldByNumber(plhs_illu_data, 0, illu_data_photons_field, plhs_illu_data_photons);
        //把数据都存到了temp里，就可以进一步存到wave里了
        mxFree(data_illu_data);
    //把做好的data加进去
    mxSetFieldByNumber(plhs_illu, 0, illu_data_field, plhs_illu_data);
    //将最终的illu放进去
    mxSetFieldByNumber(plhs, 0, illuminant_field, plhs_illu);

    //加入data结构
    mxArray *plhs_data;
    //加入spectrum，这在某种意义上是递归加入的
    const char* data_field_names[] = {"photons","luminance"};
    int data_num_of_field =  (sizeof(data_field_names) / sizeof(*data_field_names));
    //该情况下dims应该都可以通用
    mwSize dims_data[2] = {1, 1};
    plhs_data = mxCreateStructArray(2,dims_data,data_num_of_field,data_field_names);
    //luminance就是个[],不需要添加，放着就行
    //添加三维的photons
    //注意matlab都是height，width，1这样在c++中十分不方便，我一律存为1，height,width
    //radiance的存储也因此变成了： C0i,height,width,而matlab中应该是height,width,C0i
        long unsigned int data_photons_field = mxGetFieldNumber(plhs_data, "photons");
        long unsigned int height_photons = scene.data.photons.at(0).size();
        long unsigned int width_photons = scene.data.photons.at(0).at(0).size();
        long unsigned int samples_photons = scene.data.photons.size();
        mwSize dims_photons[3] = {height_photons,width_photons,samples_photons};
        double *data_data_photons = (double*)mxMalloc(sizeof(double)*height_photons*width_photons*samples_photons);
        mxArray *photons_array = mxCreateNumericArray(3,dims_photons,mxDOUBLE_CLASS, mxREAL);
        //存储优先级：先y再x再z
        for(int x=0;x<width_photons;x++){
            for(int y = 0;y<height_photons;y++){
                for(int z =0;z<samples_photons;z++){
                    double result_temp = scene.data.photons.at(z).at(y).at(x);
                    data_data_photons[z*width_photons*height_photons + x*height_photons+y] = scene.data.photons.at(z).at(y).at(x);
                    // data_data_photons[z*width_photons*height_photons + y*height_photons+y] = scene.data.photons.at(z).at(y).at(x);
                }
            }
        }
        // for(int x=0;x<depth_width;x++){
        // for(int y=0;y<depth_height;y++){
        //     data_depth[x*depth_height+y] = scene.depthMap.at(0).at(y).at(x);
        // }
        // }
        memcpy((void *)(mxGetPr(photons_array)), (void *)data_data_photons, sizeof(double)*height_photons*width_photons*samples_photons);
        mxSetFieldByNumber(plhs_data, 0, data_photons_field, photons_array);
        mxFree(data_data_photons);
    mxSetFieldByNumber(plhs, 0, data_field, plhs_data);


//     // 创建MATLAB三维数组。在传递指定维度大小的数组时，按 Y,X,Z的顺序设置。即矩阵为Y行，X列，共Z个矩阵
//     size_t dims[3] = { 3, 4, 2 };
//     mxArray *mat_array = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    
//     double * dst = (double *)(mxGetPr(pMxArray));
//     double * d = dst;
//     for (int z = 0; z < 2; ++z) {
//         // MATLAB按列优先存储
//         for (int x = 0; x < 4; ++x) {
//             for (int y = 0; y < 3; ++y) {
//                 (*d++) = src[z * 3 * 4 + y * 4 + x];
//             }
//         }
//     }
    status = matPutVariable(pmat, "scene", plhs);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }




    // mxArray *pa1;
    // pa1 = mxCreateDoubleMatrix(3,3,mxREAL);
    // if (pa1 == NULL) {
    //     printf("%s : Out of memory on line %d\n", __FILE__, __LINE__); 
    //     printf("Unable to create mxArray.\n");
    //     return(EXIT_FAILURE);
    // }
    // status = matPutVariable(pmat, "LocalDouble", pa1);
    // if (status != 0) {
    //     printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
    //     return(EXIT_FAILURE);
    // }
    // /* clean up */
    // mxDestroyArray(pa1);
    
    // mxArray *pa2;
    // pa2 = mxCreateDoubleMatrix(3,3,mxREAL);
    // if (pa2 == NULL) {
    //     printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
    //     printf("Unable to create mxArray.\n");
    //     return(EXIT_FAILURE);
    // }
    // memcpy((void *)(mxGetPr(pa2)), (void *)data, sizeof(data));
    // status = matPutVariableAsGlobal(pmat, "GlobalDouble", pa2);
    // if (status != 0) {
    //     printf("Error using matPutVariableAsGlobal\n");
    //     return(EXIT_FAILURE);
    // }
    // mxDestroyArray(pa2);
    // /*
    // * Ooops! we need to copy data before writing the array.  (Well,
    // * ok, this was really intentional.) This demonstrates that
    // * matPutVariable will overwrite an existing array in a MAT-file.
    // */
    // memcpy((void *)(mxGetPr(pa1)), (void *)data, sizeof(data));
    // status = matPutVariable(pmat, "LocalDouble", pa1);
    // if (status != 0) {
    //     printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
    //     return(EXIT_FAILURE);
    // } 

    if (matClose(pmat) != 0) {
        printf("Error closing file %s\n",file);
        return(EXIT_FAILURE);
    }
    return(EXIT_SUCCESS);
}
    // /*
    // * Re-open file and verify its contents with matGetVariable
    // */
    // pmat = matOpen(file, "r");
    // if (pmat == NULL) {
    //     printf("Error reopening file %s\n", file);
    //     return(EXIT_FAILURE);
    // }

    // /*
    // * Read in each array we just wrote
    // */
    // pa1 = matGetVariable(pmat, "LocalDouble");
    // if (pa1 == NULL) {
    //     printf("Error reading existing matrix LocalDouble\n");
    //     return(EXIT_FAILURE);
    // }
    // if (mxGetNumberOfDimensions(pa1) != 2) {
    //     printf("Error saving matrix: result does not have two dimensions\n");
    //     return(EXIT_FAILURE);
    // }

    // pa2 = matGetVariable(pmat, "GlobalDouble");
    // if (pa2 == NULL) {
    //     printf("Error reading existing matrix GlobalDouble\n");
    //     return(EXIT_FAILURE);
    // }
    // if (!(mxIsFromGlobalWS(pa2))) {
    //     printf("Error saving global matrix: result is not global\n");
    //     return(EXIT_FAILURE);
    // }

    // pa_name = matGetVariable(pmat, "name");
    // if (pa_name == NULL) {
    //     printf("Error reading existing matrix name\n");
    //     return(EXIT_FAILURE);
    // }
    
    // status = mxGetString(pa_name, str, sizeof(str));
    // if(status != 0) {
    //     printf("Not enough space. String is truncated.");
    //     return(EXIT_FAILURE);
    // }

    // /* clean up before exit */
    // mxDestroyArray(pa1);
    // mxDestroyArray(pa2);
    // mxDestroyArray(pa_name);

    // if (matClose(pmat) != 0) {
    //     printf("Error closing file %s\n",file);
    //     return(EXIT_FAILURE);
    // }
    // printf("Done\n");
    // return(EXIT_SUCCESS);
    //     return true;

// 可以创建三维数组的示例
// int main() {
//     // 填充测试数组
//     double _3d_array[2][3][4];

//     int sn = 1;
//     for (int z = 0; z < 2; ++z) {
//         for (int y = 0; y < 3; ++y) {
//             for (int x = 0; x < 4; ++x) {
//                 _3d_array[z][y][x] = sn++;
//             }
//         }
//     }

//     // 创建mat文件
//     MATFile *mat_file = matOpen("d:\\test.mat", "w");
    
//     // 创建MATLAB三维数组。在传递指定维度大小的数组时，按 Y,X,Z的顺序设置。即矩阵为Y行，X列，共Z个矩阵
//     size_t dims[3] = { 3, 4, 2 };
//     mxArray *mat_array = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    
//     double * dst = (double *)(mxGetPr(pMxArray));
//     double * d = dst;
//     for (int z = 0; z < 2; ++z) {
//         // MATLAB按列优先存储
//         for (int x = 0; x < 4; ++x) {
//             for (int y = 0; y < 3; ++y) {
//                 (*d++) = src[z * 3 * 4 + y * 4 + x];
//             }
//         }
//     }

//     // 保存三维数组
//     matPutVariable(mat_file, "test_value", mat_array);
    
//     // 释放mat文件对象
//     matClose(mat_file);
    
//     return 0;
// }


/*=================================================================
 * mxcreatestructarray.c
 *
 * mxcreatestructarray illustrates how to create a MATLAB structure
 * from a corresponding C structure.  It creates a 1-by-4 structure mxArray,
 * which contains two fields, name and phone number where name is store as a
 * string and phone number is stored as a double.  The structure that is
 * passed back to MATLAB could be used as input to the phonebook.c example
 * in $MATLAB/extern/examples/refbook.
 *
 * This is a MEX-file for MATLAB.
 * Copyright 1984-2018 The MathWorks, Inc.
 * All rights reserved.
 *=================================================================*/

// #include "mex.h"
// #include <string.h>

// #define NUMBER_OF_STRUCTS (sizeof(friends) / sizeof(struct phonebook))
// #define NUMBER_OF_FIELDS (sizeof(field_names) / sizeof(*field_names))

// struct phonebook {
//     const char* name;
//     double phone;
// };

// void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
//     const char* field_names[] = {"name", "phone"};
//     struct phonebook friends[] = {{"Jordan Robert", 3386},
//                                   {"Mary Smith", 3912},
//                                   {"Stacy Flora", 3238},
//                                   {"Harry Alpert", 3077}};
//     mwSize dims[2] = {1, NUMBER_OF_STRUCTS};
//     int name_field, phone_field;
//     mwIndex i;

//     (void)prhs;

//     /* Check for proper number of input and  output arguments */
//     if (nrhs != 0) {
//         mexErrMsgIdAndTxt("MATLAB:mxcreatestructarray:maxrhs", "No input argument required.");
//     }
//     if (nlhs > 1) {
//         mexErrMsgIdAndTxt("MATLAB:mxcreatestructarray:maxlhs", "Too many output arguments.");
//     }

//     /* Create a 1-by-n array of structs. */
//     plhs[0] = mxCreateStructArray(2, dims, NUMBER_OF_FIELDS, field_names);

//     /* This is redundant, but here for illustration.  Since we just
//        created the structure and the field number indices are zero
//        based, name_field will always be 0 and phone_field will always
//        be 1 */
//     name_field = mxGetFieldNumber(plhs[0], "name");
//     phone_field = mxGetFieldNumber(plhs[0], "phone");

//     /* Populate the name and phone fields of the phonebook structure. */
//     for (i = 0; i < NUMBER_OF_STRUCTS; i++) {
//         mxArray* field_value;
//         /* Use mxSetFieldByNumber instead of mxSetField for efficiency
//          * mxSetField(plhs[0],i,"name",mxCreateString(friends[i].name); */
//         mxSetFieldByNumber(plhs[0], i, name_field, mxCreateString(friends[i].name));
//         field_value = mxCreateDoubleMatrix(1, 1, mxREAL);
// #if MX_HAS_INTERLEAVED_COMPLEX
//         *mxGetDoubles(field_value) = friends[i].phone;
// #else
//         *mxGetPr(field_value) = friends[i].phone;
// #endif
//         /* Use mxSetFieldByNumber instead of mxSetField for efficiency
//          * mxSetField(plhs[0],i,"name",mxCreateString(friends[i].name); */
//         mxSetFieldByNumber(plhs[0], i, phone_field, field_value);
//     }
// }