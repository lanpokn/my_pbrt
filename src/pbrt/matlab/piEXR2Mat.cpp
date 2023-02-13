#include "piEXR2Mat.h"
#include <regex.h>
//注意matlab都是height，width，1这样在c++中十分不方便，我一律存为1，height,width
double piExr2Mat(string infile,string channelname){
    //TOCHECK
    string::size_type iPos = (infile.find_last_of('\\') + 1) == 0 ?  infile.find_last_of('/') + 1: infile.find_last_of('\\') + 1 ;
    string fnameTag = infile .substr(iPos, infile .length() - iPos);//获取带后缀的文件名
    string indir = infile .substr(0,iPos);//获取文件路径
    string fname = fnameTag.substr(0, fnameTag.rfind("."));//获取不带后缀的文件名
    string Tag = fname.substr(fname.rfind("."),fname.length());//获取后缀名


}
// [status,result] = system(dockercmd);



// if status
//     disp(result);
//     error('EXR to Binary conversion failed.')
// end
// filelist = dir([indir,sprintf('/%s_*',fname)]);
// nameparts = strsplit(filelist(1).name,'_');
// Nparts = numel(nameparts);
// height = str2double(nameparts{Nparts-2});
// width= str2double(nameparts{Nparts-1});

// if strcmp(channelname,'Radiance')
//     baseName = strsplit(filelist(1).name,'.');
//     for ii = 1:31
//         filename = fullfile(indir, [baseName{1},sprintf('.C%02d',ii)]);
//         [fid, message] = fopen(filename, 'r');
//         serializedImage = fread(fid, inf, 'float');
//         data(:,:,ii) = reshape(serializedImage, height, width, 1);
//         fclose(fid);
//         delete(filename);
//     end
// else
//         filename = fullfile(indir, filelist(1).name);
//         [fid, message] = fopen(filename, 'r');
        // serializedImage = fread(fid, inf, 'float');
        // data = reshape(serializedImage, height, width, 1);
//         fclose(fid);
//         delete(filename);
// end