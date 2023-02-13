#ifndef MatDs
#define MatDs
#include<string>
#include<vector>

//约定，多重vector,如双重photons(1)，对应phtons(1,:,:)
using namespace std;
//注意,虽然这里没有强行要求,但是photons之类的一定是强行对齐好的
namespace MatDS{
//为了兼容性，depthmat也变成三维，注意第一维度必须为1，即只有depth.at(0)
typedef vector<vector<vector<double>>> Photons;
typedef vector<vector<vector<double>>> Energy;
typedef vector<vector<vector<double>>> DepthMap;
struct Scene
{
    // %% Set the photons into the scene
    // scene.type = 'scene';
    string type;
    // scene.metadata = [];
    vector<double> metadata;//似乎什么都不同给u,空着就行
    // scene.spectrum.wave = p.Results.wavelength;
    Spectrum spectrum;
    // scene.distance = 1.0;
    double distance;
    // scene.magnification = 1;
    double magnification = 1;
    // scene.data.photons = photons;
    // scene.data.luminance = [];
    Data data;
    // scene.illuminant.name = 'D65';
    // scene.illuminant.type = 'illuminant';
    // scene.illuminant.spectrum.wave = [400,410,420,430,440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,590,600,610,620,630,640,650,660,670,680,690,700];
    // scene.illuminant.data.photons = [9.0457595e+15;1.0250447e+16;1.0724296e+16;1.0186997e+16;1.2611661e+16;1.4392664e+16;1.4814279e+16;1.4757830e+16;1.5211861e+16;1.4576585e+16;1.4949163e+16;1.5032161e+16;1.4899500e+16;1.5606805e+16;1.5416749e+16;1.5648686e+16;1.5313995e+16;1.5016408e+16;1.5193658e+16;1.4309865e+16;1.4769715e+16;1.4948381e+16;1.4871616e+16;1.4351952e+16;1.4652141e+16;1.4228330e+16;1.4481596e+16;1.5079676e+16;1.4562064e+16;1.3159828e+16;1.3712425e+16];
    Illuminant illuminant;
    // scene.wAngular = 40;
    double wAngular;
    // [r,c] = size(photons(:,:,1)); depthMap = ones(r,c);
    // scene.depthMap = depthMap;
    DepthMap depthMap;
    string name;
    Scene(){
        //
    }
};
struct Spectrum{
    Wave wave;
    Spectrum(){
        wave = (Wave)"400:10:700";
    }
};

struct Data
{
    //photons 似乎是个三维数组,光子数估计还是有可能是小数,让他们取整
    Photons photons;
    vector<double> luminance;
};

struct Illuminant
{
    string name;
    string type;
    Spectrum spectrum;
    Data data;
};
//默认16通道
struct Wave
{
    vector<double> wave;
    Wave(){

    }
    //以下构造函数用于一步创建31通道的wave,因为要么31要么16,变化不大
    Wave(string& s){
        if(s == "400:10:700"){
            for(int i = 400;i<710;i = i+10){
                wave.push_back(i);
            }
        }else{
            for(int i = 400;i<710;i = i+20){
                wave.push_back(i);
            }
        }
    }
    Wave(string&& sr){
        string s = move(sr);
        if(s == "400:10:700"){
            for(int i = 400;i<710;i = i+10){
                wave.push_back(i);
            }
        }else{
            for(int i = 400;i<710;i = i+20){
                wave.push_back(i);
            }
        }
    }

};


}


#endif