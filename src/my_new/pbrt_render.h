#ifndef MY_PBRT_RENDER_H
#define MY_PBRT_RENDER_H
#include<my_new/render.h>


#include <pbrt/pbrt.h>

#include <pbrt/cpu/render.h>
#ifdef PBRT_BUILD_GPU_RENDERER
#include <pbrt/gpu/memory.h>
#endif  // PBRT_BUILD_GPU_RENDERER
#include <pbrt/options.h>
#include <pbrt/parser.h>
#include <pbrt/scene.h>
#include <pbrt/util/args.h>
#include <pbrt/util/check.h>
#include <pbrt/util/error.h>
#include <pbrt/util/log.h>
#include <pbrt/util/memory.h>
#include <pbrt/util/parallel.h>
#include <pbrt/util/print.h>
#include <pbrt/util/spectrum.h>
#include <pbrt/util/string.h>
#include <pbrt/wavefront/wavefront.h>


#include <ImfChannelList.h>
#include <ImfChromaticitiesAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfFrameBuffer.h>
#include <ImfHeader.h>
#include <ImfInputFile.h>
#include <ImfIntAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfOutputFile.h>
#include <ImfStringAttribute.h>
#include <ImfStringVectorAttribute.h>

#include <string>
#include <vector>
#include <iostream>
using namespace pbrt;

//use it to memory the exr data
//use static to avoid multi defination
//use extern to fix this problem!
namespace pbrt_render_h{
extern Imf::FrameBuffer EXRFrameBuffer;
extern Point2i resolution;
extern Imf::Header header;
}

struct PbrtConfig:BasicConfig{
    //cmd 
    int seed = 0;
    bool quiet = false;
    bool disablePixelJitter = false, disableWavelengthJitter = false;
    bool disableTextureFiltering = false;
    bool forceDiffuse = false;
    bool useGPU = false;
    bool wavefront = false;
    bool interactive = false;
    RenderingCoordinateSystem renderingSpace = RenderingCoordinateSystem::CameraWorld;
    
    int nThreads = 0;
    LogLevel logLevel = LogLevel::Error;
    std::string logFile;
    bool logUtilization = false;
    bool writePartialImages = false;
    bool recordPixelStatistics = false;
    bool printStatistics = false;
    pstd::optional<int> pixelSamples;
    pstd::optional<int> gpuDevice;
    bool quickRender = false;
    bool upgrade = false;
    std::string imageFile;
    std::string mseReferenceImage, mseReferenceOutput;
    std::string debugStart;
    std::string displayServer;
    pstd::optional<Bounds2f> cropWindow;
    pstd::optional<Bounds2i> pixelBounds;
    pstd::optional<Point2i> pixelMaterial;
    Float displacementEdgeScale = 1;
    
    //it will be used when parsefile, if it is not null, we will create a new pbrt configfile(with a different name), and then delete it
    //because original file is very conplex and only use a filename as input, do so can avoid change code
    //为什么要生成新的文件：原本的源代码涉及到多个文件，而且最后应该是用的重定向输入流的方式进行读取，要改动就要涉及其底层逻辑
    //而且要么改动一堆源文件，要么该文件会在其他地方出现难以预料的问题
    //因此，我决定生成一个临时配置文件，至于是否需要运行后删除就看需求了
    //所以接下来的任务：读取源文件，在保证源文件,修改其中特定的camera内容(可以考虑先把camera后边所有开头是空格的行，以及camera所在行“”的内容删除，再加），再生成到新的配置文件里，
    //然后在parsefile前根据需求更改filevector（有理由认为原作者考虑过多相机，不然为什么要用vector?)
    //记得跑一下有多个相机定义的文件，看看是怎么回事
    //TODO
    //vector<CheckInfo*> info;
    std::vector<RealisticCameraParam> RealCameraList;
    std::vector<PerspectiveCameraParam> PerspectiveCameraList;
    std::vector<SphericalCameraParam> SphericalCameraList;
    std::vector<OrthographicCameraParam> OrthographicCameraList;
    void AddRealCamera(float shutteropen = 0 ,float shutterclose = 1,std::string lensfile = "",float aperturediameter = 1.0,
                       float focusdistance = 10.0, std::string aperture = "circular")
    { 
        std::string label_input = "RealCamera_"+std::to_string(RealCameraList.size());
        RealisticCameraParam t(shutteropen,shutterclose,label_input,lensfile,aperturediameter,focusdistance,aperture);
        RealCameraList.push_back(t);
    }
    void AddPerspectiveCamera(float shutteropen = 0 ,float shutterclose = 1, float frameaspectratio_input = -1,
                              float screenwindow_input  = -1.0,float lensradius_input = 1,
                              float focaldistance_input =-1,float fov_input=90)
    {
        std::string label_input = "PerspectiveCamera_"+std::to_string(PerspectiveCameraList.size());
        PerspectiveCameraParam t(shutteropen,shutterclose,label_input,frameaspectratio_input,screenwindow_input,lensradius_input,focaldistance_input,fov_input);
        PerspectiveCameraList.push_back(t);
    }
    void AddSphericalCamera(float shutteropen_input = 0,float shutterclose_input = 1,
                    std::string mapping_input = "equalarea")
    {
        std::string label_input = "SphericalCamera_"+std::to_string(SphericalCameraList.size());
        SphericalCameraParam t(shutteropen_input,shutterclose_input,label_input,mapping_input);
        SphericalCameraList.push_back(t);
    }
    void AddOrthographicCamera(float shutteropen_input = 0,float shutterclose_input = 1,
                    float frameaspectratio_input = -1,float screenwindow_input  = -1.0,float lensradius_input = 1,
                    float focaldistance_input = -1)
    {
        std::string label_input = "OrthographicCamera_"+std::to_string(OrthographicCameraList.size());
        OrthographicCameraParam t(shutteropen_input,shutterclose_input,label_input,frameaspectratio_input,screenwindow_input,lensradius_input,focaldistance_input);
        OrthographicCameraList.push_back(t);
    }
    /**
     * @brief convert it to a cmd string
     * 
     * @return std::string 
     */
    virtual std::string ToString() const;
};
/**
 * @brief render an image with pbrt method
 * 
 */
class pbrt_render:render{
  private:
    std::string cmd_input = "";
    //these params have to be add because they are not one of  command line
    std::vector<RealisticCameraParam> RealCameraList;
    std::vector<PerspectiveCameraParam> PerspectiveCameraList;
    std::vector<SphericalCameraParam> SphericalCameraList;
    std::vector<OrthographicCameraParam> OrthographicCameraList;
    /**
     * @brief copy from the pbrt,
     * 
     * @param argv the input of the cmd
     * @return int 
     */
    int pbrt_main(char *argv[]);
    /**
     * @brief generate a new .pbrt file and renturn the new filename
     * 
     * @param RC 
     * @param filenames 
     */
   std::string generateFilenames(std::string filenames,std::string suffix);
   std::string generatePbrtFile(RealisticCameraParam RC, std::string filenames);
   std::string generatePbrtFile(PerspectiveCameraParam PC, std::string filenames);
   std::string generatePbrtFile(OrthographicCameraParam OC, std::string filenames);
   std::string generatePbrtFile(SphericalCameraParam SC, std::string filenames);
  public:
    Imf::FrameBuffer fb;
    Point2i resolution;
    Imf::Header header;
    /**
     * @brief change config to string, and then call init(str)
     * 
     * @param con the config edit by the user
     * @return true 
     * @return false 
     */
    virtual bool init(PbrtConfig &con);
    /**
     * @brief get a string from the user, then run() will change it to char *argv[] which can be understood by the pbrt_main
     * 
     * @param str the str should be the same as the cmd input, such as --display-server localhost:14158 scene/explosion/explosion.pbrt
     *            these should be space between words
     * @return true 
     * @return false 
     */
    virtual bool init(std::string &str);
    /**
     * @brief just need to call the pbrt_main with current cmd_input(change to char*[])
     *        anytime before calling a run, there should be an init() before it
     *        TODO:change run() to avoid change cmd_vel when dealing with it
     * 
     * @return true 
     * @return false 
     */
    virtual bool init(std::string &&str);
    virtual bool run();
};

#endif  //MY_PBRT_RENDER_H