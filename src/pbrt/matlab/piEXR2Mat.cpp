#include "piEXR2Mat.h"

#include <regex.h>
//注意matlab都是height，width，1这样在c++中十分不方便，我一律存为1，height,width
//radiance的存储也因此变成了： C0i,height,width,注意最后存的时候能对的上即可
Energy piEXR2Mat(string infile,string channelname){
    //TOCHECK
    Energy data;
    allCHA Metadata;
    MyImgTool(infile,channelname,Metadata);
    if("Radiance" == channelname){
        //i表示第i通道
        for(int i=0;i<Metadata.allData.size();i++){
            //temp1存height，width
            vector<vector<double>> temp1;
            for(int y =0;y<Metadata.height;y++){
                //temp2存一个width
                vector<double> temp2;
                for(int x=0;x<Metadata.width;x++){
                    //                 buf_exr[x * res.y + y] =
                    // image.GetChannel({x, y}, exr2mat_channels.at(c) - 1);
                    temp2.push_back(Metadata.allData.at(i)[x*Metadata.height+y]);
                }
                temp1.push_back(temp2);
            }
            data.push_back(temp1);
        }
    } else{
        //该方法应该是普适的，因为当Px,Py,Pz等情况，正好size()为1
        for(int i=0;i<Metadata.allData.size();i++){
            //temp1存height，width
            vector<vector<double>> temp1;
            for(int y =0;y<Metadata.height;y++){
                //temp2存一个width
                vector<double> temp2;
                for(int x=0;x<Metadata.width;x++){
                    //                 buf_exr[x * res.y + y] =
                    // image.GetChannel({x, y}, exr2mat_channels.at(c) - 1);
                    temp2.push_back(Metadata.allData.at(i)[x*Metadata.height+y]);
                }
                temp1.push_back(temp2);
            }
            data.push_back(temp1);
        }
    }
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