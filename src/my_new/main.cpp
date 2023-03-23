#include<my_new/pbrt_render.h>
#include<iostream>
using namespace std;
//TODO:给init增加右值引用的功能，否则无法直接输入一个字符串给函数，使用上有严重缺失
int main(){
    //init a render to do the main loop
    pbrt_render render;
    //init a config to set up render
    PbrtConfig con;
    // if you have not give enough msg to config, init won't work
    cout<<render.init(con)<<endl;
    //give msg to config
    con.AddRealCamera(0,3,"lenses/wide.22mm.dat",2.0);//可以添加多个camera
    con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    con.nThreads = 4;
    // con.cropWindow = myBound(0,0.5,0,0.5);
    //set up render
    render.init(con);//init（con)将相机数据传入render
    //run the render
    render.run();
    //if you needn't change camera, I suggest just do this:
    render.init("pbrt /home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt");//init（con)将相机数据传入render
    render.run();
}