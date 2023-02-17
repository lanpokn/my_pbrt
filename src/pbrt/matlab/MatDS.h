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
//默认16通道
typedef struct Wave_struct
{
    vector<double> wave;
    Wave_struct(){

    }
    //以下构造函数用于一步创建31通道的wave,因为要么31要么16,变化不大
    Wave_struct(string& s){
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
    Wave_struct(string&& sr){
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

} Wave;

typedef struct Spectrum_struct{
    Wave wave;
    Spectrum_struct(){
        wave = (Wave)"400:10:700";
    }
} Spectrum;

typedef struct Data_struct
{
    //photons 似乎是个三维数组,光子数估计还是有可能是小数,让他们取整
    Photons photons;
    vector<double> luminance;
} Data;

typedef struct Illuminant_struct
{
    string name;
    string type;
    Spectrum spectrum;
    Data data;
}Illuminant;


typedef struct Scene_struct
{
    // %% Set the photons into the scene
    // scene.type = 'scene';
    string type;
    vector<double> metadata;//似乎什么都不同给u,空着就行
    Spectrum spectrum;
    double distance;
    double magnification = 1;
    Data data;
    Illuminant illuminant;
    double wAngular;
    DepthMap depthMap;
    string name;
    Scene_struct(){
        //
    }
}Scene;

}

#endif