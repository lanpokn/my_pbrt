#include<my_new/pbrt_render.h>
#include<iostream>
using namespace std;
//TODO:给init增加右值引用的功能，否则无法直接输入一个字符串给函数，使用上有严重缺失
int main(){
    pbrt_render render;
    PbrtConfig con;
    cout<<render.init(con)<<endl;
    //TODO 要提供一个选项，当不add时应该用一次配置文件的的相机
    // con.AddPerspectiveCamera();
    // con.AddOrthographicCamera();
    // con.AddSphericalCamera();
    con.AddRealCamera(0,3,"lenses/wide.22mm.dat",2.0);//可以添加多个camera,目前以最后一个为准
    // string cmd = "--display-server localhost:14158 /home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    // string cmd = "--display-server localhost:14158 /home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/sanmiguel/sanmiguel-realistic-courtyard.pbrt";
    // render.init(cmd);
    con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    // con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/sanmiguel/sanmiguel-realistic-courtyard.pbrt";
    render.init(con);//init（con)将相机数据传入render
    render.run();
}