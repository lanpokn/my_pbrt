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
//cmd 
    // int seed = 0;
    bool quiet = false;
    // bool disablePixelJitter = false, disableWavelengthJitter = false;
    // bool disableTextureFiltering = false;
    // bool forceDiffuse = false;
    bool useGPU = false;
    bool wavefront = false;
    // bool interactive = false;
    // RenderingCoordinateSystem renderingSpace = RenderingCoordinateSystem::CameraWorld;
    
    int nThreads = 0;
    // LogLevel logLevel = LogLevel::Error;
    // std::string logFile;
    // bool logUtilization = false;
    // bool writePartialImages = false;
    // bool recordPixelStatistics = false;
    // bool printStatistics = false;
    //changed
    int pixelSamples = -1;
    int gpuDevice =  -1;
    bool quickRender = false;
    // bool upgrade = false;
    // std::string imageFile;
    // std::string mseReferenceImage, mseReferenceOutput;
    // std::string debugStart;
    // std::string displayServer;
    //changed
    myBound cropWindow;
    // Bounds2f projRearBounds(Point2f(-1.5f * rearRadius, -1.5f * rearRadius),
    //                         Point2f(1.5f * rearRadius, 1.5f * rearRadius));
    // pstd::optional<Bounds2i> pixelBounds;
    // pstd::optional<Point2i> pixelMaterial;
    // Float displacementEdgeScale = 1;
    
    //it will be used when parsefile, if it is not null, we will create a new pbrt configfile(with a different name), and then delete it
    //because original file is very conplex and only use a filename as input, do so can avoid change code
    //TODO
    //vector<CheckInfo*> info;
    std::vector<RealisticCameraParam> RealCameraList;
    std::vector<PerspectiveCameraParam> PerspectiveCameraList;
    std::vector<SphericalCameraParam> SphericalCameraList;
    std::vector<OrthographicCameraParam> OrthographicCameraList;
    //output
    Image image;
    ImageChannelDesc desc;
    Imath::Box2i dataWindow;
    int resolutiony;
    Imf::Header header;
    Imf::FrameBuffer imageToFrameBuffer(const Image &image,
                                           const ImageChannelDesc &desc,
                                           const Imath::Box2i &dataWindow);
    void WriteExr(std::string name);
    void AddRealCamera(float shutteropen = 0 ,float shutterclose = 1,
                     std::string lensfile = "",float diagonal_input = 35,
                     std::string Look_at = "",
                     float aperturediameter = 1.0,
                       float focusdistance = 10.0, std::string aperture = "circular")
    { 
        std::string label_input = "RealCamera_"+std::to_string(RealCameraList.size());
        RealisticCameraParam t(shutteropen,shutterclose,label_input,Look_at,diagonal_input,lensfile,aperturediameter,focusdistance,aperture);
        RealCameraList.push_back(t);
    }
    void AddPerspectiveCamera(float shutteropen = 0 ,float shutterclose = 1,
                              std::string Look_at = "",
                              float frameaspectratio_input = -1,
                              float screenwindow_input  = -1.0,float lensradius_input = 1,
                              float focaldistance_input =-1,float fov_input=90)
    {
        std::string label_input = "PerspectiveCamera_"+std::to_string(PerspectiveCameraList.size());
        PerspectiveCameraParam t(shutteropen,shutterclose,label_input,Look_at,frameaspectratio_input,screenwindow_input,lensradius_input,focaldistance_input,fov_input);
        PerspectiveCameraList.push_back(t);
    }
    void AddSphericalCamera(float shutteropen_input = 0,float shutterclose_input = 1,
                    std::string Look_at = "",
                    std::string mapping_input = "equalarea")
    {
        std::string label_input = "SphericalCamera_"+std::to_string(SphericalCameraList.size());
        SphericalCameraParam t(shutteropen_input,shutterclose_input,Look_at,label_input,mapping_input);
        SphericalCameraList.push_back(t);
    }
    void AddOrthographicCamera(float shutteropen_input = 0,float shutterclose_input = 1,
                    std::string Look_at = "",
                    float frameaspectratio_input = -1,float screenwindow_input  = -1.0,float lensradius_input = 1,
                    float focaldistance_input = -1)
    {
        std::string label_input = "OrthographicCamera_"+std::to_string(OrthographicCameraList.size());
        OrthographicCameraParam t(shutteropen_input,shutterclose_input,Look_at,label_input,frameaspectratio_input,screenwindow_input,lensradius_input,focaldistance_input);
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
  public:
    std::vector<PbrtConfig> Configlist;
    bool is_run = false;
    /**
     * @brief change config to string, and then call init(str)
     * 
     * @param con the config edit by the user
     * @return true 
     * @return false 
     */
    virtual bool AddConfig(PbrtConfig& con);
    /**
     * @brief get a string from the user, then run() will change it to char *argv[] which can be understood by the pbrt_main
     *        can add multi time, but these settings will all use the first con(except scene path) because initPBRT can only use once
     * @param str the str should be the same as the cmd input, such as --display-server localhost:14158 scene/explosion/explosion.pbrt
     *            these should be space between words
     * @return true 
     * @return false 
     */
    virtual bool initWithCmd(std::string &str);
    /**
     * @brief just need to call the pbrt_main with current cmd_input(change to char*[])
     *        anytime before calling a run, there should be an init() before it
     *        TODO:change run() to avoid change cmd_vel when dealing with it
     * 
     * @return true 
     * @return false 
     */
    virtual bool initWithCmd(std::string &&str);
    virtual bool run();
};