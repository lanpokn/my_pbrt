from setuptools import setup, Extension
import numpy
import cv2

# These paths should be changed to your own paths
module = Extension("sgrb2lum",
                   sources=["C:/Users/admin/Documents/2023/PBES/my_pbrt/IEBCS-main/cpp/utils/display.cpp"],
                   include_dirs=[numpy.get_include(), "C:/Users/admin/Desktop/code/opencv/build/include/"],
                   library_dirs=["C:/Users/admin/Desktop/code/opencv/build/x64/vc16/bin/", "C:/Users/admin/Desktop/code/opencv/build/x64/vc16/lib/", "C:/Users/admin/Desktop/code/opencv/build/x64/vc16/"],
                   libraries=["opencv_world470d"],
                  #  extra_compile_args=["/std:c++11"],
                  #  extra_link_args=["/NODEFAULTLIB:MSVCRT", "/NODEFAULTLIB:LIBCMT", "/MT"],
                   language="c++")

setup(
    name="sgrb2lum",
    version="1.0",
    description="Converts SRGB to luminance",
    ext_modules=[module]
)