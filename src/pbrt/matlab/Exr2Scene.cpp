//约定，depth不存在时不要给，这样可以通过size为0来判断
#include<cmath>

#include"Exr2Scene.h"

#include"ieParamFormat.h"
#include "piReadEXR.h"
#include "Energy2Quanta.h"
#include "piSceneCreate.h"
#include "find_fov.h"
Scene Exr2Scene(string inputFile,string pbrt_path,Wave wave){
    Scene scene;
    return scene;
    Energy energy = piReadEXR(inputFile,"radiance");//seg fault
    bool Is_16 = false;
    if(energy.at(0).at(0).size()<17){
        Is_16 = true;
    }else{
        Is_16 = true;
        for(int i= 0;i<energy.size();i++){
            for(int j = 0;j<energy.size();j++){
                if(energy.at(i).at(j).at(16)!=0){
                    Is_16 = false;
                    break;
                }
            }
        }
    }

    Wave data_wave = (Wave)"400:10:700";
    if(true == Is_16){
         
    // for(int i= 0;i<energy.size();i++){
    //     for(int j = 0;j<energy.size();j++){
            
    //     }
    // }
     
        for(int i= 0;i<energy.size();i++){
            for(int j = 0;j<energy.size();j++){
                while(energy.at(i).at(j).size()>16){
                    energy.at(i).at(j).pop_back();
                }
            }
        } 
        Wave data_wave = (Wave)"400:20:700";
    }
    Photons photons = Energy2Quanta(data_wave, energy); 
    DepthMap depthmap = piReadEXR(inputFile,"depth");

    // %%
    // % Create a name for the ISET object
    // 为了方便我就随便来了
    string ieObjName = inputFile.substr(0,inputFile.size()-4);
    string cameraType = "perspective";
    //这里虽然看起来像是要分开处理，但实际上realistic相机根本就没做
    Scene ieObject = piSceneCreate(photons,40,100,data_wave);
    ieObject.name = ieObjName;

    if(!pbrt_path.empty()){
        //59行这里，死活认不出来
        double hh = photons.size();
        double ww = photons.at(0).size();
        double cc = photons.at(0).at(0).size();
        double fov = find_fov(pbrt_path);
        //??TOCHECK why do ??twice
        double pi = 3.1415926535;
        double wAngular = atan(tan(fov*pi/(2*180))*ww/hh)/pi*180*2;
        ieObject.wAngular = wAngular;
        ieObject.wAngular = 0;
    }

    //get depth,
    //不存在depthmat 时，此时depthmap给全1的单通道矩阵即可
    //大小和energy对齐
    if(0 == depthmap.size()){
        vector<vector<double>> temp1;
        for(int i= 0;i<energy.at(0).size();i++){
            vector<double> temp2;
            for(int j = 0;j<energy.at(0).at(0).size();j++){
                temp2.push_back(1);
            }
            temp1.push_back(temp2);
        }
        depthmap.push_back(temp1);
    }
    ieObject.depthMap = depthmap;
    //在此处赋值默认值
    ieObject.illuminant.data.photons = ieObject.data.photons;
    ieObject.illuminant.spectrum= ieObject.spectrum;
    ieObject.illuminant.name = "D65";
    ieObject.illuminant.type = "illuminant"; 
    return ieObject;
}