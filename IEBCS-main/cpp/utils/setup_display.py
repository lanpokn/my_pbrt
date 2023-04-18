from setuptools import setup, Extension
import numpy
import cv2

module = Extension("sgrb2lum",
                   sources=["/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/IEBCS-main/cpp/utils/display.cpp"],
                #    include_dirs=[numpy.get_include(), cv2.get_include()],
                   include_dirs=[numpy.get_include(),"/usr/include/opencv4"],
                   library_dirs = ["/usr/lib/x86_64-linux-gnu","/usr/local/lib"],
                   libraries=["opencv_features2d", 
                           "opencv_highgui", "opencv_core"],
                   extra_compile_args=["-std=c++11"],
                   extra_link_args=["-std=c++11"])

setup(
    name="sgrb2lum",
    version="1.0",
    description="Converts SRGB to luminance",
    ext_modules=[module],
    # cmdclass={"build_ext": build_ext},
)

# import cv2
# print(cv2.__version__)
# print(cv2.getBuildInformation())