#include"piReadEXR.h"
#include<cmath>
//读取深度时如果读不到就cout，跳过，不输入深度
#include"ieParamFormat.h"
#include "piEXR2Mat.h"
Energy piReadEXR(string filename, string datatype){
    // filename = ieParamFormat(filename); 这东西不能标准化，标准之后就找不到了
    datatype = ieParamFormat(datatype);
    //所有输出都可以先被统一成三通道，所以output就用energy了
    Energy output;
    if("radiance" == datatype){
        output = piEXR2Mat(filename,"Radiance");
    }else    if("zdepth" == datatype){
        output = piEXR2Mat(filename,"Pz");
    }else     if("depth" == datatype){
        Energy XDepthMap = piEXR2Mat(filename,"Px");
        Energy YDepthMap = piEXR2Mat(filename,"Py");
        Energy ZDepthMap = piEXR2Mat(filename,"Pz");
        //if no PZ ,break
        if(ZDepthMap.size() == 0){
            cout<<"no PZ"<<endl;
            return output;
        }
        vector<vector<double>> temp1;
        for(int i =0;i<XDepthMap.at(0).size();i++){
            vector<double> temp2;
            for(int j =0;j<XDepthMap.at(0).at(0).size();j++){
                double x = XDepthMap.at(0).at(i).at(j);
                double y = YDepthMap.at(0).at(i).at(j);
                double z = ZDepthMap.at(0).at(i).at(j);
                temp2.push_back(sqrt(x*x+y*y+z*z));
            }
            temp1.push_back(temp2);
        }
        output.push_back(temp1);
    }else    if("3dcoordinates" == datatype){
        // output = piReadEXR(filename,"Radiance");
        //这个有点难做，暂时不实现，因为用不到
    } else{
        cout<<"no invalid input in piReadEXR"<<endl;
    }
    return output;
    //    case "material" % single channel
    //     output = piEXR2Mat(filename, 'MaterialId');
    // case "normal"
    //     output(:,:,1) = piEXR2Mat(filename, 'Nx');
    //     output(:,:,2) = piEXR2Mat(filename, 'Ny');
    //     output(:,:,3) = piEXR2Mat(filename, 'Nz');
    // case "albedo"
    //     % to add; only support rgb for now, spectral albdeo needs to add;
    // case "instance" % single channel
    //     output = piEXR2Mat(filename, 'InstanceId');
}