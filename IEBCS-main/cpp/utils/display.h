/**
 * @file Display.h
 * @brief class of display
 * @mainpage
 * @author weihl@THUJIGROUP
 * @email weihenglu@yeah.net
 * @version 1.0.0
 * @date 2023-1-12
 */
#pragma once
#include<vector>
#include "opencv2/opencv.hpp"
#include "../scene/SceneRGB.h"
#include "../scene/SceneSpectral.h"
#include "COLORSPACE.h"
class SceneRGB;
class SceneSpectral;
class Display
{
public:
    virtual ~Display() {};
    virtual SceneSpectral rendering(const SceneRGB& rgb, std::vector<float> waveDst = std::vector<float>()) const = 0;
    virtual COLORSPACE getChroma() const = 0;
    virtual float getMaxLuminance() const = 0;
private:
};

class DisplaySRGB : public Display
{
public:
    DisplaySRGB() = delete;
    DisplaySRGB(const std::vector<float>& ww, const std::vector<cv::Point3f>& ss, const std::vector<cv::Point3f>& gg);
    virtual ~DisplaySRGB() {}
    virtual SceneSpectral rendering(const SceneRGB& rgb, std::vector<float> waveDst = std::vector<float>()) const override;
    virtual COLORSPACE getChroma() const override { return cs; };
    virtual float getMaxLuminance() const override { return maxLuminance; }
private:
    std::vector<float> wave;
    std::vector<cv::Point3f> spd;
    std::vector<cv::Point3f> gamma;
    COLORSPACE cs;
    float maxLuminance;
};