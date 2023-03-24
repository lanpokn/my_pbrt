#include<my_new/utilsFunc.h>

std::string generateFilenames(std::string filenames,std::string suffix = "_user"){
    std::string newfilenames = filenames.substr(0, filenames.length()-5)+suffix + ".pbrt";
    return newfilenames;
}
// this need to be overload to different versions
//hhq
std::string generatePbrtFile(RealisticCameraParam RC, std::string filenames){
    std::string newfilenames;
    // newfilenames = filenames.substr(0, filenames.length()-5)+"_user.pbrt";
    newfilenames = generateFilenames(filenames,"_user");
    std::ifstream infile(filenames);//需要修改的文件。
    std::ofstream outfile(newfilenames);//out.txt可以自动创建，每次运行自动覆盖。

    int findCamera = 0;//0:have not findCamera 1:finding 2:have find 3:finding another Camera
    int lineAfterCamera = 0;
    std::string temp;
    std::string templast;
    //解释：后边几个类似，原理是找到camera的位置，然后变成自己的camera，在所有信息都结束之后break出来
    //因为有些不好给默认值，所以缺省参数为-1，用浮点数比较，比-0.9大的时候认为是用户在输入，此时在写入，否则不写入，让系统用默认值
    //因为这些数一定不是负数，所以这样做是有效的。
    //注意了，label是我自己加的，想要区分不同相机的参数，不能输入到.pbrt中
    while (!infile.eof())
    {
        getline(infile,temp); //用string中的getline方法，获取infile中的一行，到temp变量中，getline()会去除最后的换行符。
        //first: judge the state,be sure findCamera will only be changed per line
        if("Camera"==temp.substr(0,6)){
            if(0 == findCamera){
                findCamera = 1;
            }
            else if(2 == findCamera){
                findCamera = 3;
            }
        }
        else if((1 == findCamera or 3 == findCamera) and ' '!=temp[0])
        {
            findCamera = 2;
        }

        //then:output file
        if(0 ==findCamera or 2 == findCamera ){
            outfile << temp << std::endl;
        }
        else if(3 == findCamera){
            //do not output for now, because I don't know why there would be two Camera setting
            continue;
        }
        else if (1 == findCamera)
        {
            //TODO, if you need to overload
            while(1){
                if(0 == lineAfterCamera){
                    temp = "Camera \"realistic\"";
                }
                else if (1 == lineAfterCamera)
                {
                    temp = "    \"float shutteropen\" [ "+ std::to_string(RC.shutteropen)  +" ]";
                }
                else if (2 == lineAfterCamera)
                {
                    temp = "    \"float shutterclose\" [ "+ std::to_string(RC.shutterclose)  +" ]";
                }
                else if (3 == lineAfterCamera)
                {
                    temp ="    \"string lensfile\" [ \""+RC.lensfile+"\" ]";
                }
                else if (4 == lineAfterCamera)
                {
                    temp ="    \"float aperturediameter\" [ "+std::to_string(RC.aperturediameter)+" ]";
                }
                else if (5 == lineAfterCamera)
                {
                    temp ="    \"float focusdistance\" [ "+std::to_string(RC.focusdistance)+" ]";
                }
                else if (6 == lineAfterCamera)
                {
                    if("circular"!=RC.aperture){
                        temp ="    \"string aperture\" [ "+RC.aperture+" ]";
                        outfile << temp << std::endl;
                        break;
                    }
                    else{
                        break;
                    }
                }
                lineAfterCamera++;
                outfile << temp << std::endl;
            }
            //after output, delete other lines until all the original Camera msg is removed 
            findCamera = 3;
        }
    }
    infile.close();
    outfile.close();
    return newfilenames;
}
std::string generatePbrtFile(PerspectiveCameraParam PC, std::string filenames){

    std::string newfilenames;
    newfilenames = generateFilenames(filenames,"_user");
    std::ifstream infile(filenames);//需要修改的文件。
    std::ofstream outfile(newfilenames);//out.txt可以自动创建，每次运行自动覆盖。

    int findCamera = 0;//0:have not findCamera 1:finding 2:have find 3:finding another Camera
    int lineAfterCamera = 0;
    std::string temp;
    std::string templast;
    while (!infile.eof())
    {
        getline(infile,temp); //用string中的getline方法，获取infile中的一行，到temp变量中，getline()会去除最后的换行符。
        //first: judge the state,be sure findCamera will only be changed per line
        if("Camera"==temp.substr(0,6)){
            if(0 == findCamera){
                findCamera = 1;
            }
            else if(2 == findCamera){
                findCamera = 3;
            }
        }
        else if((1 == findCamera or 3 == findCamera) and ' '!=temp[0])
        {
            findCamera = 2;
        }

        //then:output file
        if(0 ==findCamera or 2 == findCamera ){
            outfile << temp << std::endl;
        }
        else if(3 == findCamera){
            //do not output for now, because I don't know why there would be two Camera setting
            continue;
        }
        else if (1 == findCamera)
        {
            //TODO, if you need to overload
            while(1){
                if(0 == lineAfterCamera){
                    temp = "Camera \"perspective\"";
                }
                else if (1 == lineAfterCamera)
                {
                    temp = "    \"float shutteropen\" [ "+ std::to_string(PC.shutteropen)  +" ]";
                }
                else if (2 == lineAfterCamera)
                {
                    temp = "    \"float shutterclose\" [ "+ std::to_string(PC.shutterclose)  +" ]";
                }
                else if (3 == lineAfterCamera)
                {
                temp ="    \"float fov\" [ "+std::to_string(PC.fov)+" ]";
                }
                else if (4 == lineAfterCamera)
                {
                    temp ="    \"float lensradius\" [ "+std::to_string(PC.lensradius)+" ]";
                }
                else if (5 == lineAfterCamera)
                {   
                    if(-0.9<PC.focaldistance){
                        temp ="    \"float focaldistance\" [ "+std::to_string(PC.focaldistance)+" ]";
                    }
                    else{

                    }
                }
                else if (6 == lineAfterCamera)
                {
                    if(-0.9<PC.frameaspectratio){
                        temp ="    \"float frameaspectratio\" [ "+std::to_string(PC.frameaspectratio)+" ]";
                    }
                    else{
                    }
                }
                else if (7 == lineAfterCamera)
                {
                    if(-0.9<PC.screenwindow){
                        temp ="    \"float screenwindow\" [ "+std::to_string(PC.screenwindow)+" ]";
                        outfile << temp << std::endl;
                        break;
                    }
                    else{
                        break;
                    }
                }
                lineAfterCamera++;
                outfile << temp << std::endl;
            }
            //after output, delete other lines until all the original Camera msg is removed 
            findCamera = 3;
        }
    }
    infile.close();
    outfile.close();
    return newfilenames;
}
std::string generatePbrtFile(OrthographicCameraParam OC, std::string filenames){
    std::string newfilenames;
    newfilenames = generateFilenames(filenames,"_user");
    std::ifstream infile(filenames);//需要修改的文件。
    std::ofstream outfile(newfilenames);//out.txt可以自动创建，每次运行自动覆盖。

    int findCamera = 0;//0:have not findCamera 1:finding 2:have find 3:finding another Camera
    int lineAfterCamera = 0;
    std::string temp;
    std::string templast;
    while (!infile.eof())
    {
        getline(infile,temp); //用string中的getline方法，获取infile中的一行，到temp变量中，getline()会去除最后的换行符。
        //first: judge the state,be sure findCamera will only be changed per line
        if("Camera"==temp.substr(0,6)){
            if(0 == findCamera){
                findCamera = 1;
            }
            else if(2 == findCamera){
                findCamera = 3;
            }
        }
        else if((1 == findCamera or 3 == findCamera) and ' '!=temp[0])
        {
            findCamera = 2;
        }

        //then:output file
        if(0 ==findCamera or 2 == findCamera ){
            outfile << temp << std::endl;
        }
        else if(3 == findCamera){
            //do not output for now, because I don't know why there would be two Camera setting
            continue;
        }
        else if (1 == findCamera)
        {
            //TODO, if you need to overload
            while(1){
                if(0 == lineAfterCamera){
                    temp = "Camera \"orthographic\"";
                }
                else if (1 == lineAfterCamera)
                {
                    temp = "    \"float shutteropen\" [ "+ std::to_string(OC.shutteropen)  +" ]";
                }
                else if (2 == lineAfterCamera)
                {
                    temp = "    \"float shutterclose\" [ "+ std::to_string(OC.shutterclose)  +" ]";
                }
                else if (3 == lineAfterCamera)
                {
                    temp ="    \"float lensradius\" [ "+std::to_string(OC.lensradius)+" ]";
                }
                else if (4 == lineAfterCamera)
                {
                    if(-0.9<OC.focaldistance){
                        temp ="    \"float focaldistance\" [ "+std::to_string(OC.focaldistance)+" ]";
                    }
                    else{

                    }
                }
                else if (5 == lineAfterCamera)
                {
                    if(-0.9<OC.frameaspectratio){
                        temp ="    \"float frameaspectratio\" [ "+std::to_string(OC.frameaspectratio)+" ]";
                    }
                    else{
                    }
                }
                else if (6 == lineAfterCamera)
                {
                    if(-0.9<OC.screenwindow){
                        temp ="    \"float screenwindow\" [ "+std::to_string(OC.screenwindow)+" ]";
                        outfile << temp << std::endl;
                        break;
                    }
                    else{
                        break;
                    }
                }
                lineAfterCamera++;
                outfile << temp << std::endl;
                temp = "";
            }
            //after output, delete other lines until all the original Camera msg is removed 
            findCamera = 3;
        }
    }
    infile.close();
    outfile.close();
    return newfilenames;
}
std::string generatePbrtFile(SphericalCameraParam SC, std::string filenames){
    std::string newfilenames;
    newfilenames = generateFilenames(filenames,"_user");
    std::ifstream infile(filenames);//需要修改的文件。
    std::ofstream outfile(newfilenames);//out.txt可以自动创建，每次运行自动覆盖。

    int findCamera = 0;//0:have not findCamera 1:finding 2:have find 3:finding another Camera
    int lineAfterCamera = 0;
    std::string temp;
    std::string templast;
    while (!infile.eof())
    {
        getline(infile,temp); //用string中的getline方法，获取infile中的一行，到temp变量中，getline()会去除最后的换行符。
        //first: judge the state,be sure findCamera will only be changed per line
        if("Camera"==temp.substr(0,6)){
            if(0 == findCamera){
                findCamera = 1;
            }
            else if(2 == findCamera){
                findCamera = 3;
            }
        }
        else if((1 == findCamera or 3 == findCamera) and ' '!=temp[0])
        {
            findCamera = 2;
        }

        //then:output file
        if(0 ==findCamera or 2 == findCamera ){
            outfile << temp << std::endl;
        }
        else if(3 == findCamera){
            //do not output for now, because I don't know why there would be two Camera setting
            continue;
        }
        else if (1 == findCamera)
        {
            //TODO, if you need to overload
            while(1){
                if(0 == lineAfterCamera){
                    temp = "Camera \"spherical\"";
                }
                else if (1 == lineAfterCamera)
                {
                    temp = "    \"float shutteropen\" [ "+ std::to_string(SC.shutteropen)  +" ]";
                }
                else if (2 == lineAfterCamera)
                {
                    temp = "    \"float shutterclose\" [ "+ std::to_string(SC.shutterclose)  +" ]";
                }
                else if (3 == lineAfterCamera)
                {
                    if("equalarea"< SC.mapping){
                        temp ="    \"string mapping\" [ "+SC.mapping+" ]";
                        outfile << temp << std::endl;
                        break;
                    }
                    else{
                        break;
                    }
                }
                lineAfterCamera++;
                outfile << temp << std::endl;
            }
            //after output, delete other lines until all the original Camera msg is removed 
            findCamera = 3;
        }
    }
    infile.close();
    outfile.close();
    return newfilenames;
}
