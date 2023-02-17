#include<string>
#include<cstdlib>
//尽量使用绝对路径
void RunPbrt(std::string &str){
    //小米的任务十分简单，所以我直接执行预先编译的可执行文件了
    //其实就是个system
    std::string input = str;
    const char *s = input.data();
    system(s);
}
//对右值进行支持
void RunPbrt(std::string &&str){
    //小米的任务十分简单，所以我直接执行预先编译的可执行文件了
    //其实就是个system
    std::string input = std::move(str);
    const char *s = input.data();
    system(s);
}