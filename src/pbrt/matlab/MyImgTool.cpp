#include"MyImgTool.h"
#include<vector>
using namespace pbrt;

allCHA MyImgTool(string inFile,string channnelName){
    ImageAndMetadata imRead = Image::Read(inFile);
    Image image = std::move(imRead.image);
    ImageMetadata metadata = std::move(imRead.metadata);
    std::string outFile;
    std::string colorspace;
    std::string channelNames;
    std::vector<std::string> targetChannelNames;
    std::vector<int> exr2mat_channels;
    //add by hhq
    targetChannelNames.push_back(channnelName);

    Point2i res = image.Resolution();
    int nc = image.NChannels();
    std::vector<std::string> exrChannelNames = image.ChannelNames();
    if (exr2mat_channels.empty() && targetChannelNames.empty())
        for (int i = 0; i != nc; ++i) {
            exr2mat_channels.push_back(i + 1);
        }
    else if (exr2mat_channels.empty() && !targetChannelNames.empty()) {
        for (auto target : targetChannelNames) {
            for (int i = 0; i < exrChannelNames.size(); ++i) {
                auto name = exrChannelNames.at(i);
                if ((!name.compare(target)) ||
                    ((name.find(target) != std::string::npos) &&
                    (name.at(name.find(target) + target.size()) == '.'))) {
                    exr2mat_channels.push_back(i + 1);
                }
            }
        }
    }
    int mc = exr2mat_channels.size();
    size_t datasize = res.x * res.y;
    allCHA ret;
    //在这之前是确定输出的通道数，以拿到特定通道的数据，转成bin文件
    //如果转成mat，考虑替换掉最后file.write()部分即可
    //问题:会无视第二个参数,一口气都输出出来
    if (inFile.find(".exr") == inFile.npos ||
        inFile.find(".exr") != (inFile.size() - 4)) {
        fprintf(stderr, "Wrong input filename: %s  \n", inFile.c_str());
        return ret;
    }
    for (int c = 0; c < mc; ++c) {
        float *buf_exr = new float[datasize];
        for (int y = 0; y < res.y; ++y)
            for (int x = 0; x < res.x; ++x) {
                buf_exr[x * res.y + y] =
                    image.GetChannel({x, y}, exr2mat_channels.at(c) - 1);
            }
        std::size_t dirOffset;
        std::string outDir;
        std::string fileName;
        if (!outFile.empty()) {
            dirOffset = outFile.find_last_of("/\\");
            outDir = outFile.substr(0, dirOffset + 1);
            fileName = outFile.substr(dirOffset + 1);
            if (fileName.empty())
                fileName = inFile.substr(0, inFile.size() - 4);
            else if ((fileName.find(".bin") != fileName.npos) ||
                    (fileName.find(".dat") != fileName.npos))
                fileName = fileName.substr(0, fileName.size() - 4);
            else
                fileName = outDir + fileName;
        }
        else
            fileName = inFile.substr(0, inFile.size() - 4);
        std::string binaryName = fileName + '_' +
                                std::to_string(res.y) + '_' + std::to_string(res.x) +
                                '_' + exrChannelNames.at(exr2mat_channels.at(c)-1);
        //'_' + std::to_string(exr2mat_channels.at(c));
        std::fstream file(binaryName, std::ios::out | std::ios::binary);
        if (!file) {
            fprintf(stderr, "Failed opening binary file.  \n");
            return ret;
        }
        ret.allData.push_back(buf_exr);
        ret.allname.push_back(binaryName);
        // file.write((char *)buf_exr, datasize * sizeof(float));
        // file.close();
        // delete[] buf_exr;
        // buf_exr = NULL;
    }
    printf("exr2bin done.\n");
    return ret;
}