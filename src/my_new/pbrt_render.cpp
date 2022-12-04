#include<my_new/pbrt_render.h>
using namespace pbrt;

//initialize it , or cmake will throw erorr: undefined referrence
// no extern is dingyi and shengming?
Imf::FrameBuffer pbrt_render_h::EXRFrameBuffer;
Point2i pbrt_render_h::resolution;
Imf::Header pbrt_render_h::header;

static void usage(const std::string &msg = {}) {
    if (!msg.empty())
        fprintf(stderr, "pbrt: %s\n\n", msg.c_str());

    fprintf(stderr,
            R"(usage: pbrt [<options>] <filename.pbrt...>

Rendering options:
  --cropwindow <x0,x1,y0,y1>    Specify an image crop window w.r.t. [0,1]^2.
  --debugstart <values>         Inform the Integrator where to start rendering for
                                faster debugging. (<values> are Integrator-specific
                                and come from error message text.)
  --disable-pixel-jitter        Always sample pixels at their centers.
  --disable-texture-filtering   Point-sample all textures.
  --disable-wavelength-jitter   Always sample the same %d wavelengths of light.
  --displacement-edge-scale <s> Scale target triangle edge length by given value.
                                (Default: 1)
  --display-server <addr:port>  Connect to display server at given address and port
                                to display the image as it's being rendered.
  --force-diffuse               Convert all materials to be diffuse.)"
#ifdef PBRT_BUILD_GPU_RENDERER
            R"(
  --gpu                         Use the GPU for rendering. (Default: disabled)
  --gpu-device <index>          Use specified GPU for rendering.)"
#endif
            R"(
  --help                        Print this help text.
  --interactive                 Enable interactive rendering mode.
  --mse-reference-image         Filename for reference image to use for MSE computation.
  --mse-reference-out           File to write MSE error vs spp results.
  --nthreads <num>              Use specified number of threads for rendering.
  --outfile <filename>          Write the final image to the given filename.
  --pixel <x,y>                 Render just the specified pixel.
  --pixelbounds <x0,x1,y0,y1>   Specify an image crop window w.r.t. pixel coordinates.
  --pixelmaterial <x,y>         Print information about the material visible in the
                                center of the pixel's extent.
  --pixelstats                  Record per-pixel statistics and write additional images
                                with their values.
  --quick                       Automatically reduce a number of quality settings
                                to render more quickly.
  --quiet                       Suppress all text output other than error messages.
  --render-coord-sys <name>     Coordinate system to use for the scene when rendering,
                                where name is "camera", "cameraworld", or "world".
  --seed <n>                    Set random number generator seed. Default: 0.
  --stats                       Print various statistics after rendering completes.
  --spp <n>                     Override number of pixel samples specified in scene
                                description file.
  --wavefront                   Use wavefront volumetric path integrator.
  --write-partial-images        Periodically write the current image to disk, rather
                                than waiting for the end of rendering. Default: disabled.

Logging options:
  --log-file <filename>         Filename to write logging messages to. Default: none;
                                messages are printed to standard error. Implies
                                --log-level verbose if specified.
  --log-level <level>           Log messages at or above this level, where <level>
                                is "verbose", "error", or "fatal". Default: "error".
  --log-utilization             Periodically print processor and memory use in verbose-
                                level logging.

Reformatting options:
  --format                      Print a reformatted version of the input file(s) to
                                standard output. Does not render an image.
  --toply                       Print a reformatted version of the input file(s) to
                                standard output and convert all triangle meshes to
                                PLY files. Does not render an image.
  --upgrade                     Upgrade a pbrt-v3 file to pbrt-v4's format.
)",
            NSpectrumSamples);
    exit(msg.empty() ? 0 : 1);
}
std::string PbrtConfig::ToString() const{
    //there is no better way, just add one by one?
    std::string ret = "";
    //此处有两种方法，一种是无脑一波流，全转成string，另一种是比较有哪一项发生了更改，再决定加什么，我推荐前者，这样也方便其他地方用这个函数debug

    //TODO
    ret.append(this->scene_path);
    return ret;
}

// this need to be overload to four version
std::string pbrt_render::generatePbrtFile(RealisticCameraParam RC, std::string filenames){
    std::string newfilenames;
    newfilenames = filenames.substr(0, filenames.length()-5)+"_user.pbrt";
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
int pbrt_render::pbrt_main(char *argv[]){
    // Convert command-line arguments to vector of strings
    std::vector<std::string> args = GetCommandLineArguments(argv);
    std::cout<<argv<<"\n";
    // Declare variables for parsed command line
    PBRTOptions options;
    std::vector<std::string> filenames;
    std::string logLevel = "error";
    std::string renderCoordSys = "cameraworld";
    bool format = false, toPly = false;

    // Process command-line arguments
    for (auto iter = args.begin(); iter != args.end(); ++iter) {
        if ((*iter)[0] != '-') {
            filenames.push_back(*iter);
            continue;
        }

        auto onError = [](const std::string &err) {
            usage(err);
            exit(1);
        };

        std::string cropWindow, pixelBounds, pixel, pixelMaterial;
        if (ParseArg(&iter, args.end(), "cropwindow", &cropWindow, onError)) {
            std::vector<Float> c = SplitStringToFloats(cropWindow, ',');
            if (c.size() != 4) {
                usage("Didn't find four values after --cropwindow");
                return 1;
            }
            options.cropWindow = Bounds2f(Point2f(c[0], c[2]), Point2f(c[1], c[3]));
        } else if (ParseArg(&iter, args.end(), "pixel", &pixel, onError)) {
            std::vector<int> p = SplitStringToInts(pixel, ',');
            if (p.size() != 2) {
                usage("Didn't find two values after --pixel");
                return 1;
            }
            options.pixelBounds =
                Bounds2i(Point2i(p[0], p[1]), Point2i(p[0] + 1, p[1] + 1));
        } else if (ParseArg(&iter, args.end(), "pixelbounds", &pixelBounds, onError)) {
            std::vector<int> p = SplitStringToInts(pixelBounds, ',');
            if (p.size() != 4) {
                usage("Didn't find four integer values after --pixelbounds");
                return 1;
            }
            options.pixelBounds = Bounds2i(Point2i(p[0], p[2]), Point2i(p[1], p[3]));
        } else if (ParseArg(&iter, args.end(), "pixelmaterial", &pixelMaterial,
                            onError)) {
            std::vector<int> p = SplitStringToInts(pixelMaterial, ',');
            if (p.size() != 2) {
                usage("Didn't find two values after --pixelmaterial");
                return 1;
            }
            options.pixelMaterial = Point2i(p[0], p[1]);
        } else if (
#ifdef PBRT_BUILD_GPU_RENDERER
            ParseArg(&iter, args.end(), "gpu", &options.useGPU, onError) ||
            ParseArg(&iter, args.end(), "gpu-device", &options.gpuDevice, onError) ||
#endif
            ParseArg(&iter, args.end(), "debugstart", &options.debugStart, onError) ||
            ParseArg(&iter, args.end(), "disable-pixel-jitter",
                     &options.disablePixelJitter, onError) ||
            ParseArg(&iter, args.end(), "disable-texture-filtering",
                     &options.disableTextureFiltering, onError) ||
            ParseArg(&iter, args.end(), "disable-wavelength-jitter",
                     &options.disableWavelengthJitter, onError) ||
            ParseArg(&iter, args.end(), "displacement-edge-scale",
                     &options.displacementEdgeScale, onError) ||
            ParseArg(&iter, args.end(), "display-server", &options.displayServer,
                     onError) ||
            ParseArg(&iter, args.end(), "force-diffuse", &options.forceDiffuse,
                     onError) ||
            ParseArg(&iter, args.end(), "format", &format, onError) ||
            ParseArg(&iter, args.end(), "log-level", &logLevel, onError) ||
            ParseArg(&iter, args.end(), "log-utilization", &options.logUtilization,
                     onError) ||
            ParseArg(&iter, args.end(), "log-file", &options.logFile, onError) ||
            // ParseArg(&iter, args.end(), "interactive", &options.interactive, onError) ||
            ParseArg(&iter, args.end(), "mse-reference-image", &options.mseReferenceImage,
                     onError) ||
            ParseArg(&iter, args.end(), "mse-reference-out", &options.mseReferenceOutput,
                     onError) ||
            ParseArg(&iter, args.end(), "nthreads", &options.nThreads, onError) ||
            ParseArg(&iter, args.end(), "outfile", &options.imageFile, onError) ||
            ParseArg(&iter, args.end(), "pixelstats", &options.recordPixelStatistics,
                     onError) ||
            ParseArg(&iter, args.end(), "quick", &options.quickRender, onError) ||
            ParseArg(&iter, args.end(), "quiet", &options.quiet, onError) ||
            ParseArg(&iter, args.end(), "render-coord-sys", &renderCoordSys, onError) ||
            ParseArg(&iter, args.end(), "seed", &options.seed, onError) ||
            ParseArg(&iter, args.end(), "spp", &options.pixelSamples, onError) ||
            ParseArg(&iter, args.end(), "stats", &options.printStatistics, onError) ||
            ParseArg(&iter, args.end(), "toply", &toPly, onError) ||
            ParseArg(&iter, args.end(), "wavefront", &options.wavefront, onError) ||
            ParseArg(&iter, args.end(), "write-partial-images",
                     &options.writePartialImages, onError) ||
            ParseArg(&iter, args.end(), "upgrade", &options.upgrade, onError)) {
            // success, but do nothing, we have got all useful msg and put it into options
        } else if (*iter == "--help" || *iter == "-help" || *iter == "-h") {
            usage();
            return 0;
        } else {
            usage(StringPrintf("argument \"%s\" unknown", *iter));
            return 1;
        }
    }

    // Print welcome banner
    if (!options.quiet && !format && !toPly && !options.upgrade) {
        printf("pbrt version 4 (built %s at %s)\n", __DATE__, __TIME__);
#ifdef PBRT_DEBUG_BUILD
        LOG_VERBOSE("Running debug build");
        printf("*** DEBUG BUILD ***\n");
#endif
        printf("Copyright (c)1998-2021 Matt Pharr, Wenzel Jakob, and Greg Humphreys.\n");
        printf("The source code to pbrt (but *not* the book contents) is covered "
               "by the Apache 2.0 License.\n");
        printf("See the file LICENSE.txt for the conditions of the license.\n");
        fflush(stdout);
    }

    // Check validity of provided arguments
    if (renderCoordSys == "camera")
        options.renderingSpace = RenderingCoordinateSystem::Camera;
    else if (renderCoordSys == "cameraworld")
        options.renderingSpace = RenderingCoordinateSystem::CameraWorld;
    else if (renderCoordSys == "world")
        options.renderingSpace = RenderingCoordinateSystem::World;
    else
        ErrorExit("%s: unknown rendering coordinate system.", renderCoordSys);

    if (!options.mseReferenceImage.empty() && options.mseReferenceOutput.empty())
        ErrorExit("Must provide MSE reference output filename via "
                  "--mse-reference-out");
    if (!options.mseReferenceOutput.empty() && options.mseReferenceImage.empty())
        ErrorExit("Must provide MSE reference image via --mse-reference-image");

    if (options.pixelMaterial && options.useGPU) {
        Warning("Disabling --use-gpu since --pixelmaterial was specified.");
        options.useGPU = false;
    }

    if (options.useGPU && options.wavefront)
        Warning("Both --gpu and --wavefront were specified; --gpu takes precedence.");

    if (options.pixelMaterial && options.wavefront) {
        Warning("Disabling --wavefront since --pixelmaterial was specified.");
        options.wavefront = false;
    }

    // if (options.interactive && !(options.useGPU || options.wavefront))
    //     ErrorExit("The --interactive option is only supported with the --gpu "
    //               "and --wavefront integrators.");

    options.logLevel = LogLevelFromString(logLevel);

    // Initialize pbrt
    InitPBRT(options);
    // pbrt就两种大功能，一种用来渲染图片，一种用来转化图片
    if (format || toPly || options.upgrade) {
        FormattingParserTarget formattingTarget(toPly, options.upgrade);
        ParseFiles(&formattingTarget, filenames);
    } else {
        // Parse provided scene description files
        BasicScene scene;
        BasicSceneBuilder builder(&scene);
        if(RealCameraList.size()!= 0){
            //use user define .pbrt file instead of default one
            RealisticCameraParam RC = RealCameraList.back();
            std::string new_filename;
            new_filename = this->generatePbrtFile(RC,filenames.back());
            filenames.pop_back();
            filenames.push_back(new_filename);
            RealCameraList.pop_back();
        }
        ParseFiles(&builder, filenames);

        // Render the scene
        if (Options->useGPU || Options->wavefront)
            RenderWavefront(scene);
        else
            RenderCPU(scene);

        LOG_VERBOSE("Memory used after post-render cleanup: %s", GetCurrentRSS());
        // Clean up after rendering the scene
        CleanupPBRT();
    }
    return 0;
}
bool pbrt_render::init(std::string &str){
    this->cmd_input = str;
    return true;
}
bool pbrt_render::init(std::string &&str){
    this->cmd_input = std::move(str);
    return true;
}
bool pbrt_render::init(PbrtConfig &con){
//this is a tiring but easy jobs
    RealCameraList = con.RealCameraList;
    OrthographicCameraList = con.OrthographicCameraList;
    PerspectiveCameraList = con.PerspectiveCameraList;
    SphericalCameraList = con.SphericalCameraList;
    if(" "== con.scene_path){
        std::cout<<"you have not provide scene_path "<<std::endl;
        return false;
    }
    this->cmd_input = con.ToString();
    return true;
}
bool pbrt_render::run(){
    //借助strtok实现split
    //need to be test!
    //config varibles must be less than 100,and string openation won't cost too much
    char *argv[300];
    char *s = this->cmd_input.data();
    const char *d = " ";
    char *p;
    /* 获取第一个子字符串 */
    // there is no need to use _r, because we only need to split it with space
    p = strtok(s,d);
    /* 继续获取其他的子字符串 */
    //would p point to the same memory?
    int i = 0;
    argv[i] = p;
    //the first argument is excutebale path, and it is useless now, so we can set an arbitary string
    i++;
    argv[i] = p;
    while(p)
    {
        i++;
        p=strtok(NULL,d);
        argv[i] = p;
    }
    //change the order, make it the same with the argv of the cmd input    
    while(!RealCameraList.empty() or !PerspectiveCameraList.empty() or !OrthographicCameraList.empty() or !SphericalCameraList.empty())
    {
        this->pbrt_main(argv);//camera will be poped here
        // Imf::OutputFile file(name.c_str(), header);
        // file.setFrameBuffer(fb);
        // file.writePixels(resolution.y);
        // get the exr file:

        //TODO
        this->fb = pbrt_render_h::EXRFrameBuffer;
        this->resolution = pbrt_render_h::resolution;
        this->header = pbrt_render_h::header;
        // //test,name is whatever
        // std::string name = "explosion2.exr";
        // Imf::OutputFile file(name.c_str(), this->header);
        // file.setFrameBuffer(this->fb);
        // file.writePixels(this->resolution.y);
    }
}