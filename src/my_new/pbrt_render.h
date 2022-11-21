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
extern int numscanlines;
extern Imf::Header header;
}

struct PbrtConfig:BasicConfig{
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
    string cmd_input = "";
    /**
     * @brief copy from the pbrt,
     * 
     * @param argv the input of the cmd
     * @return int 
     */
    int pbrt_main(char *argv[]);
  public:
    Imf::FrameBuffer fb;
    int numscanlines = 1;
    Imf::Header header;
    /**
     * @brief change config to string, and then call init(str)
     * 
     * @param con the config edit by the user
     * @return true 
     * @return false 
     */
    virtual bool init(BasicConfig &con);
    /**
     * @brief get a string from the user, then run() will change it to char *argv[] which can be understood by the pbrt_main
     * 
     * @param str the str should be the same as the cmd input, such as --display-server localhost:14158 scene/explosion/explosion.pbrt
     *            these should be space between words
     * @return true 
     * @return false 
     */
    virtual bool init(string &str);
    /**
     * @brief just need to call the pbrt_main with current cmd_input(change to char*[])
     *        anytime before calling a run, there should be an init() before it
     *        TODO:change run() to avoid change cmd_vel when dealing with it
     * 
     * @return true 
     * @return false 
     */
    virtual bool init(string &&str);
    virtual bool run();
};

#endif  //MY_PBRT_RENDER_H