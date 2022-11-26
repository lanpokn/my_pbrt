#include<my_new/pbrt_render.h>
#include<iostream>
using namespace std;
//TODO:给init增加右值引用的功能，否则无法直接输入一个字符串给函数，使用上有严重缺失
int main(){
    pbrt_render render;
    PbrtConfig con;
    cout<<render.init(con)<<endl;
    con.AddRealCamera();
    con.AddRealCamera(2,3);
    cout<<con.RealCameraList.at(0).aperturediameter<<endl;
    // string cmd = "--display-server localhost:14158 /home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    // string cmd = "--display-server localhost:14158 /home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/sanmiguel/sanmiguel-realistic-courtyard.pbrt";
    // render.init(cmd);
    con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    render.init(con);
    render.run();
}