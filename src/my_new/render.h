#ifndef MY_RENDER_H
#define MY_RENDER_H
#include<iostream>
#include<vector>

//reference one config may have different ,we need to decompose!
/**
 * @brief config locate a lot of settings and give it to the render，note thta there is no virtual varible
 * 
 */
//config设计时要保证用户可以用最基本的操作对config进行设计，而不需要引入额外库，最简单的方法就是看这些pbrt自己定义的类的构造变量都用了哪些数据
struct BasicConfig{
    std::string scene_path;
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

    // ProjectiveCamera(CameraBaseParameters baseParameters,
    //                  const Transform &screenFromCamera, Bounds2f screenWindow,
    //                  Float lensRadius, Float focalDistance)
    //     : CameraBase(baseParameters),
    //       screenFromCamera(screenFromCamera),
    //       lensRadius(lensRadius),
    //       focalDistance(focalDistance)
struct CameraParam{
    float shutteropen = 0;
    float shutterclose = 1;
    CameraParam(float shutteropen_input = 0,float shutterclose_input = 1){
        shutteropen = shutteropen_input;
        shutterclose = shutterclose_input;
    }
};
struct RealisticCameraParam:CameraParam{
    std::string lensfile = "";//use relative path
    float aperturediameter = 1.0;
    float focusdistance = 10.0;
    // Allows specifying the shape of the camera aperture, 
    // which is circular by default. The values of "gaussian", "square", "pentagon", and "
    // star" are associated with built-in aperture shapes; o
    // ther values are interpreted as filenames specifying an image to be used to specify the shape.
    std::string aperture = "circular";

    RealisticCameraParam(float shutteropen_input = 0,float shutterclose_input = 1,
                        std::string lensfile_input = "",float aperdia_input = 1.0,float focusd_input = 10.0,
                        std::string aperture_input = "circular")
                        :CameraParam(shutteropen_input,shutterclose_input)
    {
        lensfile = lensfile_input;
        aperturediameter = aperdia_input;
        focusdistance = focusd_input;
        aperture = aperture_input;
    }
};
struct PerspectiveCameraParam:CameraParam{
    float frameaspectratio = -1;//no need to input 
    float screenwindow = -1;//no need to input 
    float lensradius = 1;
    float focaldistance = 30;
    float fov=90;
    PerspectiveCameraParam(float shutteropen_input = 0,float shutterclose_input = 1,
                    float frameaspectratio_input = -1,float screenwindow_input  = -1.0,float lensradius_input = 1,
                    float focaldistance_input = 30,float fov_input=90)
                    :CameraParam(shutteropen_input,shutterclose_input)
    {
        frameaspectratio = frameaspectratio_input;
        screenwindow = screenwindow_input;
        lensradius = lensradius_input;
        focaldistance = focaldistance_input;
        fov = fov_input;
    }
};
struct SphericalCameraParam:CameraParam{
    std::string mapping = "equalarea";
      SphericalCameraParam(float shutteropen_input = 0,float shutterclose_input = 1,
                    std::string mapping_input = "equalarea")
                    :CameraParam(shutteropen_input,shutterclose_input)
    {
        mapping = mapping_input;
    }
};
struct OrthographicParam:CameraParam{
    float frameaspectratio;//no need to input 
    float screenwindow;//no need to input 
    float lensradius = 1;
    float focaldistance = 30;
    OrthographicParam(float shutteropen_input = 0,float shutterclose_input = 1,
                    float frameaspectratio_input = -1,float screenwindow_input  = -1.0,float lensradius_input = 1,
                    float focaldistance_input = 30)
                    :CameraParam(shutteropen_input,shutterclose_input)
    {
        frameaspectratio = frameaspectratio_input;
        screenwindow = screenwindow_input;
        lensradius = lensradius_input;
        focaldistance = focaldistance_input;
    }
};
/**
 * @brief totaly virtual class, won't have a instance
 * 
 */
class render{
  public:
    // /**
    //  * @brief set all the parameter with a config struct
    //  * 
    //  * @param con  set by the users
    //  * @return true 
    //  * @return false 
    //  */
    // virtual bool init(BasicConfig &con) = 0;
    /**
     * @brief set all the parameter with a string, such as a cmd in pbrt
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    virtual bool init(std::string &str) = 0;
    /**
     * @brief almost the same with upper one
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    virtual bool init(std::string &&str) = 0;
    /**
     * @brief after init, use this function to render a picture
     * 
     * @return true 
     * @return false 
     */
    virtual bool run() = 0;
  
};

#endif