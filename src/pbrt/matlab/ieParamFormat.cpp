#include<string>
#include<algorithm>
#include<iostream>
#include<vector>
using namespace std;

#include"ieParamFormat.h"

string ClearAllSpace(string str) {
    int index = 0;
    if (!str.empty()) {
        //string::npos 表示查找没有匹配
        while((index = str.find(' ', index)) != string::npos) {
            str.erase(index, 1);
        }
    }
    return str;
}
//用重载处理matlab中的多输入
std::string ieParamFormat(std::string& s){
    string ret = s;
    //LOWER case
    if(s.empty()){
        return s;
    }
    transform(ret.begin(),ret.end(),ret.begin(),::tolower);
    //删除空格
    ret = ClearAllSpace(ret);
    return ret;
}

//如果需要用到cell，一律vector处理
vector<std::string>ieParamFormat(vector<std::string> sVec){
    vector<std::string> ret;
    for(auto i = sVec.begin();i!=sVec.end();i++){
        ret.push_back(ieParamFormat(*i));
    }
    return ret;
}