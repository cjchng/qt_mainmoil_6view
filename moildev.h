#ifndef MOILDEV_H
#define MOILDEV_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "moildev_global.h"
#include "configdata.h"
#include <opencv2/opencv.hpp>

#define PI 3.1415926
#define PCT_UNIT_WIDTH 1.27
#define PCT_UNIT_HEIGHT 1.27
#define FOCAL_LENGTH_FOR_ZOOM 250

using namespace std;
using namespace cv;
MOILDEVSHARED_EXPORT int moil_max(int num1, int num2);
class MOILDEVSHARED_EXPORT Moildev
{
private:

public:
    class ConfigData* configData;

    Moildev();
    bool Config(string cameraName, double cameraSensorWidth, double cameraSensorHeight,
        double iCx, double iCy, double i_ratio,
        double imageWidth, double imageHeight, double calibrationRatio,
        double para0, double para1, double para2, double para3, double para4, double para5
        );
    double getImageHeight();
    double getImageWidth();
    double getiCy();
    ConfigData *getcd();

    void Rotate(Mat& src, Mat& dst, double angle);
    double AnyPoint(Mat &mapX, Mat &mapY, double alphaOffset, double betaOffset, double zoom, double magnification);
    double AnyPointM(float *mapX, float *mapY, int w, int h, double alphaOffset, double betaOffset, double zoom, double magnification);
    double Panorama(Mat &mapX, Mat &mapY, double magnification /*, Mat &getAlpha, Mat &getBeta*/ );
    double Panorama(Mat &mapX, Mat &mapY, double maxAlpha, double minAlpha, double nowAlpha, Mat &getAlphaForAny, Mat &getBetaForAny, double magnification);
    double Remap(Mat &Src, Mat &Dest, Mat &mapX, Mat &mapY);

    int Max(int num1, int num2);
    Mat ImageRead(Mat image, string filename);
    void PixelOp(const Mat &src, Mat &dst);
    bool Blur(const Mat &src, Mat &dst);

};

#endif // MOILDEV_H
