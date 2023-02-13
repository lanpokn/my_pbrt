#include"piReadEXR.h"
#include<cmath>
//读取深度时如果读不到就cout，跳过，不输入深度
#include<ieParamFormat.h>
Energy piReadEXR(string filename, string datatype){
    filename = ieParamFormat(filename);
    datatype = ieParamFormat(datatype);
    //所有输出都可以先被统一成三通道，所以output就用energy了
    Energy output;
    if("radiance" == datatype){
        output = piReadEXR(filename,"Radiance");
    }else    if("zdepth" == datatype){
        output = piReadEXR(filename,"Pz");
    }else     if("depth" == datatype){
        Energy XDepthMap = piReadEXR(filename,"Px");
        Energy YDepthMap = piReadEXR(filename,"Py");
        Energy ZDepthMap = piReadEXR(filename,"Pz");
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
        }
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