#include<new/pbrt_render.h>
#include<iostream>
// #include<pbrt/matlab/MyImgTool.h>
// #include<pbrt/matlab/Exr2Scene.h>
// #include<pbrt/matlab/MatDS.h>
// #include<pbrt/matlab/WriteMat.h>
#include<pbrt/matlab/matCore.h>
using namespace std;
using namespace MatDS;
//TODO:给init增加右值引用的功能，否则无法直接输入一个字符串给函数，使用上有严重缺失
int main(){
    // std::string str = "--display-server localhost:14158 /home/lanpokn/Documents/2022/xiaomi/pbrt-v4/发给海前/main/文件/mcc/mcc.pbrt";
    // RunPbrt(str);
    Scene scene;
    scene = Exr2Scene("/home/lanpokn/Documents/2022/xiaomi/pbrt-v4/output.exr","/home/lanpokn/Documents/2022/xiaomi/pbrt-v4/发给海前/main/文件/mcc/mcc.pbrt");
    WriteMat(scene);
    // data = MyImgTool("/home/lanpokn/Documents/2022/xiaomi/pbrt-v4/explosion.exr","Radiance");

    // Exr2Scene("/home/lanpokn/Documents/2022/xiaomi/pbrt-v4/explosion.exr","/home/lanpokn/Documents/2022/xiaomi/pbrt-v4/scene/explosion/explosion.pbrt");
// /home/lanpokn/Documents/2022/xiaomi/pbrt-v4/发给海前/main/文件/mcc/mcc.pbrt
    cout<<"hello world"<<endl;
}