//this is a interface document

//config is a used to give paramter to runPbrt
typedef struct myBound_struct{
    float x0 = 0;
    float x1 = 1;
    float y0 = 0;
    float y1 = 1;
    myBound_struct(float x0_in = 0,float x1_in = 1,float y0_in = 0,float y1_in = 1){
        x0 = x0_in;
        x1 = x1_in;
        y0 = y0_in;
        y1 = y1_in;
    }
private:
    bool changed = false;
} myBound;
struct PbrtConfig:BasicConfig{
    //cmd paramter in pbrt, they are not all used 
    std::string scene_path;
    // int seed = 0;
    bool quiet = false;
    // bool disablePixelJitter = false, disableWavelengthJitter = false;
    // bool disableTextureFiltering = false;
    // bool forceDiffuse = false;
    bool useGPU = false;
    // bool wavefront = false;
    // bool interactive = false;
    // RenderingCoordinateSystem renderingSpace = RenderingCoordinateSystem::CameraWorld;
    
    int nThreads = 0;
    // LogLevel logLevel = LogLevel::Error;
    // std::string logFile;
    // bool logUtilization = false;
    // bool writePartialImages = false;
    // bool recordPixelStatistics = false;
    // bool printStatistics = false;
    pstd::optional<int> pixelSamples;
    pstd::optional<int> gpuDevice;
    bool quickRender = false;
    // bool upgrade = false;
    // std::string imageFile;
    // std::string mseReferenceImage, mseReferenceOutput;
    // std::string debugStart;
    // std::string displayServer;
    myBound cropWindow;
    // pstd::optional<Bounds2i> pixelBounds;
    // pstd::optional<Point2i> pixelMaterial;
    // Float displacementEdgeScale = 1;
    

    //locate camera parameters
    std::vector<RealisticCameraParam> RealCameraList;
    std::vector<PerspectiveCameraParam> PerspectiveCameraList;
    std::vector<SphericalCameraParam> SphericalCameraList;
    std::vector<OrthographicCameraParam> OrthographicCameraList;
    //they are functions to add cameras
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
    //these params have to be add manually because they are not one of  command line
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
    int pbrt_main(int argc, char *argv[]);
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
    /**
     * @brief they are output paramters, temporarily unavaluable
     * 
     */
    Imf::FrameBuffer fb;
    Point2i resolution;
    Imf::Header header;
    //these init function should be called only once
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
    /**
     * @brief after call the init , run() will run the pbrt main loop
     * 
     * @return true 
     * @return false 
     */
    virtual bool run();
};