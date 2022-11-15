#ifndef MY_RENDER_H
#define MY_RENDER_H
#include<iostream>
#include<vector>
using namespace std;

//reference one config may have different ,we need to decompose!
/**
 * @brief config locate a lot of settings and give it to the render，note thta there is no virtual varible
 * 
 */
//config设计时要保证用户可以用最基本的操作对config进行设计，而不需要引入额外库，最简单的方法就是看这些pbrt自己定义的类的构造变量都用了哪些数据
struct BasicConfig{
    // int seed = 0;
    // bool quiet = false;
    // bool disablePixelJitter = false, disableWavelengthJitter = false;
    // bool disableTextureFiltering = false;
    // bool forceDiffuse = false;
    // bool useGPU = false;
    // bool wavefront = false;
    // bool interactive = false;
    // RenderingCoordinateSystem renderingSpace = RenderingCoordinateSystem::CameraWorld;
    
    // int nThreads = 0;
    // LogLevel logLevel = LogLevel::Error;
    // std::string logFile;
    // bool logUtilization = false;
    // bool writePartialImages = false;
    // bool recordPixelStatistics = false;
    // bool printStatistics = false;
    // pstd::optional<int> pixelSamples;
    // pstd::optional<int> gpuDevice;
    // bool quickRender = false;
    // bool upgrade = false;
    // std::string imageFile;
    // std::string mseReferenceImage, mseReferenceOutput;
    // std::string debugStart;
    // std::string displayServer;
    // pstd::optional<Bounds2f> cropWindow;
    // pstd::optional<Bounds2i> pixelBounds;
    // pstd::optional<Point2i> pixelMaterial;
    // Float displacementEdgeScale = 1;

    virtual std::string ToString() const = 0;
};
/**
 * @brief totaly virtual class, won't have a instance
 * 
 */
class render{
  public:
    /**
     * @brief set all the parameter with a config struct
     * 
     * @param con  set by the users
     * @return true 
     * @return false 
     */
    virtual bool init(BasicConfig &con) = 0;
    /**
     * @brief set all the parameter with a string, such as a cmd in pbrt
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    virtual bool init(string &str) = 0;
    /**
     * @brief after init, use this function to render a picture
     * 
     * @return true 
     * @return false 
     */
    virtual bool run() = 0;
  
};

#endif