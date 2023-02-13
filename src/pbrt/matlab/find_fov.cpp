#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;

#include<sstream>
#include"find_fov.h"
#include"ieParamFormat.h"
double find_fov(string pbrt_path){
    ifstream infile;
    infile.open(pbrt_path);//需要修改的文件。

    int findCamera = 0;//0:have not findCamera 1:finding 2:have find 3:finding another Camera
    int lineAfterCamera = 0;
    std::string temp;
    std::string fov_line;
    std::string num_str;
    while (!infile.eof())
    {
        getline(infile,temp); //用string中的getline方法，获取infile中的一行，到temp变量中，getline()会去除最后的换行符。
        //先标准化，然后再看前八个是不是floatfov，最后提取数字
        temp = ieParamFormat(temp);
        //TOCHECK

        if(temp.substr(0,10) == "\"floatfov\""){
            if(temp.size()>10){
                fov_line = temp;
            }else{
                //我无法理解这里是为什么，需要问杨老师
                getline(infile,temp); 
                fov_line = temp;
                // std::cout<<"error in reading fov"<<endl;
            }
            break;
        }
    }
    for(auto i = fov_line.begin(); i!=fov_line.end();i++){
        if((*i<'9' && '0'<*i) || *i == '.'){
            num_str.push_back(*i);
        }
    }
    stringstream ss;
    ss<<num_str;
    double ret;
    ss>>ret;
    return ret;
}
// fileID = fopen(pbrt_path);
// tmp = textscan(fileID,'%s','Delimiter','\n','CommentStyle',{'#'});
// txtLines = tmp{1};
// nline = numel(txtLines);
// ii=1;
// while ii<=nline
//     thisLine = txtLines{ii};
//     if strncmp(thisLine, '"float fov"', length('"float fov"'))
//         if  length(thisLine) > length('"float fov" ')
//             fovLine = thisLine;
//         else
//             fovLine = txtLines{ii+1};
//         end
//     end
//     ii=ii+1;
// end
// fclose(fileID);
// num = regexp(fovLine, '-?\d*\.?\d*', 'match');
// fov = str2double(num);
