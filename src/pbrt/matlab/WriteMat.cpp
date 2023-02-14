#include </home/lanpokn/Documents/MATLAB/extern/include/mat.h>
#include </home/lanpokn/Documents/MATLAB/extern/include/matrix.h>
#include"MatDS.h"
#include"WriteMat.h"
#include<iostream>
#include<cstring>

using namespace std;
using namespace MatDS;
int WriteMat(Scene scene){
    //can be load properly
    MATFile *pmat;
    mxArray *pa1, *pa2, *pa3;
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

    pa1 = mxCreateDoubleMatrix(3,3,mxREAL);
    if (pa1 == NULL) {
        printf("%s : Out of memory on line %d\n", __FILE__, __LINE__); 
        printf("Unable to create mxArray.\n");
        return(EXIT_FAILURE);
    }

    pa2 = mxCreateDoubleMatrix(3,3,mxREAL);
    if (pa2 == NULL) {
        printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
        printf("Unable to create mxArray.\n");
        return(EXIT_FAILURE);
    }
    memcpy((void *)(mxGetPr(pa2)), (void *)data, sizeof(data));
    
    pa3 = mxCreateString("MATLAB: the language of technical computing");
    if (pa3 == NULL) {
        printf("%s :  Out of memory on line %d\n", __FILE__, __LINE__);
        printf("Unable to create string mxArray.\n");
        return(EXIT_FAILURE);
    }

    status = matPutVariable(pmat, "LocalDouble", pa1);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    }  
    
    status = matPutVariableAsGlobal(pmat, "GlobalDouble", pa2);
    if (status != 0) {
        printf("Error using matPutVariableAsGlobal\n");
        return(EXIT_FAILURE);
    } 
    
    status = matPutVariable(pmat, "LocalString", pa3);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    } 
    
    /*
    * Ooops! we need to copy data before writing the array.  (Well,
    * ok, this was really intentional.) This demonstrates that
    * matPutVariable will overwrite an existing array in a MAT-file.
    */
    memcpy((void *)(mxGetPr(pa1)), (void *)data, sizeof(data));
    status = matPutVariable(pmat, "LocalDouble", pa1);
    if (status != 0) {
        printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
        return(EXIT_FAILURE);
    } 
    
    /* clean up */
    mxDestroyArray(pa1);
    mxDestroyArray(pa2);
    mxDestroyArray(pa3);

    if (matClose(pmat) != 0) {
        printf("Error closing file %s\n",file);
        return(EXIT_FAILURE);
    }

    /*
    * Re-open file and verify its contents with matGetVariable
    */
    pmat = matOpen(file, "r");
    if (pmat == NULL) {
        printf("Error reopening file %s\n", file);
        return(EXIT_FAILURE);
    }

    /*
    * Read in each array we just wrote
    */
    pa1 = matGetVariable(pmat, "LocalDouble");
    if (pa1 == NULL) {
        printf("Error reading existing matrix LocalDouble\n");
        return(EXIT_FAILURE);
    }
    if (mxGetNumberOfDimensions(pa1) != 2) {
        printf("Error saving matrix: result does not have two dimensions\n");
        return(EXIT_FAILURE);
    }

    pa2 = matGetVariable(pmat, "GlobalDouble");
    if (pa2 == NULL) {
        printf("Error reading existing matrix GlobalDouble\n");
        return(EXIT_FAILURE);
    }
    if (!(mxIsFromGlobalWS(pa2))) {
        printf("Error saving global matrix: result is not global\n");
        return(EXIT_FAILURE);
    }

    pa3 = matGetVariable(pmat, "LocalString");
    if (pa3 == NULL) {
        printf("Error reading existing matrix LocalString\n");
        return(EXIT_FAILURE);
    }
    
    status = mxGetString(pa3, str, sizeof(str));
    if(status != 0) {
        printf("Not enough space. String is truncated.");
        return(EXIT_FAILURE);
    }
    if (strcmp(str, "MATLAB: the language of technical computing")) {
        printf("Error saving string: result has incorrect contents\n");
        return(EXIT_FAILURE);
    }

    /* clean up before exit */
    mxDestroyArray(pa1);
    mxDestroyArray(pa2);
    mxDestroyArray(pa3);

    if (matClose(pmat) != 0) {
        printf("Error closing file %s\n",file);
        return(EXIT_FAILURE);
    }
    printf("Done\n");
    return(EXIT_SUCCESS);
        return true;
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