#include<string>
#include<algorithm>
#include<iostream>
using namespace std;

#include"ieParamFormat.h"
#include"vcConstants.h"
#include"MatDS.h"
using namespace MatDS;
// function scene = piSceneCreate(photons,varargin)
// p.addRequired('photons',@isnumeric);
// p.addParameter('fov',40,@isscalar)               % Horizontal fov, degrees
// p.addParameter('meanluminance',100,@isscalar);
// p.addParameter('wavelength', 400:10:700, @isvector);

Scene piSceneCreate(Photons photons_in, double fov,double meanluminace , Wave wavelength){

    //matlab愿意:有其他乱七八糟的参数时都ie标准化一下
    // scene.type = 'scene';
    // scene.metadata = [];
    // scene.spectrum.wave = p.Results.wavelength;
    // scene.distance = 1.0;
    // scene.magnification = 1;
    // scene.data.photons = photons;
    // scene.data.luminance = [];
    // scene.illuminant.name = 'D65';
    // scene.illuminant.type = 'illuminant';
    // scene.illuminant.spectrum.wave = [400,410,420,430,440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,590,600,610,620,630,640,650,660,670,680,690,700];
    // scene.illuminant.data.photons = [9.0457595e+15;1.0250447e+16;1.0724296e+16;1.0186997e+16;1.2611661e+16;1.4392664e+16;1.4814279e+16;1.4757830e+16;1.5211861e+16;1.4576585e+16;1.4949163e+16;1.5032161e+16;1.4899500e+16;1.5606805e+16;1.5416749e+16;1.5648686e+16;1.5313995e+16;1.5016408e+16;1.5193658e+16;1.4309865e+16;1.4769715e+16;1.4948381e+16;1.4871616e+16;1.4351952e+16;1.4652141e+16;1.4228330e+16;1.4481596e+16;1.5079676e+16;1.4562064e+16;1.3159828e+16;1.3712425e+16];
    // scene.wAngular = 40;
    // [r,c] = size(photons(:,:,1)); depthMap = ones(r,c);
// scene.depthMap = depthMap;
    Scene scene;
    scene.metadata;//空，就暂时先别做
    scene.spectrum.wave = wavelength;
    scene.distance = 1;
    scene.magnification = 1;
    scene.data.photons  = photons_in;
    scene.illuminant.name = "D65";
    scene.illuminant.type = "illuminant";
    scene.illuminant.spectrum.wave = (Wave)"400:10:700";
    //用-1结尾，具体是怎么存的，还得看matlab,存了一个列向量，因此应该是第一维度！
    double temp[200] = {9.0457595e+15,1.0250447e+16,1.0724296e+16,1.0186997e+16,1.2611661e+16,1.4392664e+16,1.4814279e+16,1.4757830e+16,1.5211861e+16,1.4576585e+16,1.4949163e+16,1.5032161e+16,1.4899500e+16,1.5606805e+16,1.5416749e+16,1.5648686e+16,1.5313995e+16,1.5016408e+16,1.5193658e+16,1.4309865e+16,1.4769715e+16,1.4948381e+16,1.4871616e+16,1.4351952e+16,1.4652141e+16,1.4228330e+16,1.4481596e+16,1.5079676e+16,1.4562064e+16,1.3159828e+16,1.3712425e+16,-1};
    int i =0;
    vector<double> temp1;
    vector<vector<double>> temp2;
    while(temp[i]>0){
        temp1.push_back(temp[i]);
        i++;
        if(i>1000){
            break;
        }
        //之前这里写了个死循环。。。尽量不要写while,差点把电脑搞崩。
    }
    temp2.push_back(temp1);
    scene.illuminant.data.photons.push_back(temp2);
    scene.wAngular = 40;
    //TOCHECK!!!,这里非常容易弄反了，凡是涉及到这些的变量都要好好检查。
    int r = photons_in.size();
    int c = photons_in.at(0).size();

    //tocheck 确保不会过一轮之后之前的数据都丢了
    vector<vector<double>> temp3;
    for(int i =0;i<r;i++){
        vector<double> temp4;
        for(int j = 0;j<c;j++){
            temp4.push_back(1);
        }
        temp3.push_back(temp4);
    }
    scene.depthMap.push_back(temp3);
    return scene;

}
