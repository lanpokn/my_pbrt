#include<new/pbrt_render.h>
#include<iostream>
using namespace std;
//TODO:给init增加右值引用的功能，否则无法直接输入一个字符串给函数，使用上有严重缺失
int main(){
    std::string str = "--display-server localhost:14158 /home/lanpokn/Documents/2022/xiaomi/pbrt-v4/scene/explosion/explosion.pbrt";
    RunPbrt(str);
}