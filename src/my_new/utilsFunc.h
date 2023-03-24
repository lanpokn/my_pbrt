#ifndef UTILSFUNC
#define UTILSFUNC

#include<my_new/render.h>
#include <iostream>
#include<fstream>

std::string generateFilenames(std::string filenames,std::string suffix);
std::string generatePbrtFile(RealisticCameraParam RC, std::string filenames);
std::string generatePbrtFile(PerspectiveCameraParam PC, std::string filenames);
std::string generatePbrtFile(OrthographicCameraParam OC, std::string filenames);
std::string generatePbrtFile(SphericalCameraParam SC, std::string filenames);

#endif