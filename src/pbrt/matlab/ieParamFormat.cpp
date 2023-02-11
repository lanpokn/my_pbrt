#include<string>
#include<algorithm>
#include<iostream>
#include<vector>
using namespace std;

#include"ieParamFormat.h"
//用重载处理matlab中的多输入
std::string ieParamFormat(std::string& s){
    string ret = s;
    //LOWER case
    transform(ret.begin(),ret.end(),ret.begin(),::tolower);
    //删除空格
    s.erase(std::remove_if(ret.begin(), ret.end(), ::isspace), ret.end());
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