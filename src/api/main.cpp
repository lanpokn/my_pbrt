#include<api/pbrt_render.h>
#include<event/event.h>
#include<iostream>
using namespace std;
int main(){
    //init a render to do the main loop
    pbrt_render render;
    //init a config to set up render
    PbrtConfig con;
    PbrtConfig con2;
    //give msg to config
    for(int i = 1;i<2;i++){
        PbrtConfig con;
        con.AddRealCamera(0,3,"lenses/wide.22mm.dat");
        con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/cube1/cube10000"+to_string(i)+".pbrt";
        cout<<con.scene_path<<endl;
        render.AddConfig(con);
    }
    // con.AddRealCamera(0,3,"lenses/wide.22mm.dat",35,"LookAt 00 400 0       -20 0 150   0 0 1");
    // con.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    // con.nThreads = 4;
    // con2.scene_path = "/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/scene/explosion/explosion.pbrt";
    // con2.AddRealCamera(0,3,"lenses/wide.22mm.dat",75);
    // //set up render
    // render.AddConfig(con);//init（con)将相机数据传入render
    // render.AddConfig(con2);//init（con)将相机数据传入render
    //run the render
    render.run();
    // for(int i =1;i<24;i++){
    //     std::string name = "cube"+to_string(i);
    //     render.Configlist.at(i-1).WriteExr(name);
    // }
    // //运行完成后，结果一律存储在ConfigList中！所以必须传递引用，否则用户那里拿不到返回数据，除非通过configlist再去拿
    // //vector 使用深拷贝，改成引用很难，除非使用指针，否则无法把值传回来，但是用引用还过于繁琐
    // //因此返回数据放在configlist中
    // std::string name = "explosion.exr";
    // render.Configlist.at(0).WriteExr(name);
    // std::string name2 = "explosion2.exr";
    // render.Configlist.at(1).WriteExr(name2);

    GenerateIntensityVideo(render,180,130);
}