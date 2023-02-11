#ifndef IEPARAMFORMAT
#define IEPARAMFORMAT

#include<string>
#include<algorithm>
#include<iostream>
#include<vector>
using namespace std;

//用重载处理matlab中的多输入
std::string ieParamFormat(std::string& s);

//如果需要用到cell，一律vector处理
vector<std::string>ieParamFormat(vector<std::string> sVec);

#endif