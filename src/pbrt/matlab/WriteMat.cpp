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

    double data[9] = { 1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0 };
    const char *file = "mattest.mat";
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
    mxSetFieldByNumber(plhs, 0, name_field, mxCreateString(scene.name.c_str()));
    mxSetFieldByNumber(plhs, 0, type_field, mxCreateString(scene.type.c_str()));
    status = matPutVariable(pmat, "Scene", plhs);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }




    mxArray *pa1;
    pa1 = mxCreateDoubleMatrix(3,3,mxREAL);
    if (pa1 == NULL) {
        printf("%s : Out of memory on line %d\n", __FILE__, __LINE__); 
        printf("Unable to create mxArray.\n");
        return(EXIT_FAILURE);
    }
    status = matPutVariable(pmat, "LocalDouble", pa1);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }
    /* clean up */
    mxDestroyArray(pa1);
    
    mxArray *pa2;
    pa2 = mxCreateDoubleMatrix(3,3,mxREAL);
    if (pa2 == NULL) {
        printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
        printf("Unable to create mxArray.\n");
        return(EXIT_FAILURE);
    }
    memcpy((void *)(mxGetPr(pa2)), (void *)data, sizeof(data));
    status = matPutVariableAsGlobal(pmat, "GlobalDouble", pa2);
    if (status != 0) {
        printf("Error using matPutVariableAsGlobal\n");
        return(EXIT_FAILURE);
    }
    mxDestroyArray(pa2);
    
    mxArray *pa_name;
    pa_name = mxCreateString(scene.name.c_str());
    if (pa_name == NULL) {
        printf("%s :  Out of memory on line %d\n", __FILE__, __LINE__);
        printf("Unable to create string mxArray.\n");
        return(EXIT_FAILURE);
    }
    status = matPutVariable(pmat, "name", pa_name);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }
    mxDestroyArray(pa_name);   
    
    mxArray *pa_type;
    pa_type = mxCreateString(scene.type.c_str());
    if (pa_type == NULL) {
        printf("%s :  Out of memory on line %d\n", __FILE__, __LINE__);
        printf("Unable to create string mxArray.\n");
        return(EXIT_FAILURE);
    }
    status = matPutVariable(pmat, "type", pa_type);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }
    mxDestroyArray(pa_name);   
    
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
}

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