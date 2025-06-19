#ifndef MOILDEV_H
#define MOILDEV_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "moildev_global.h"
#include "configdata.h"

#include <opencv2/opencv.hpp>

#include <QPoint>
#include <QPointF>

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
    class ConfigData* cfg;

    Moildev();
    bool Config(string cameraName, double cameraSensorWidth, double cameraSensorHeight,
        double iCx, double iCy, double i_ratio,
        double imageWidth, double imageHeight, double calibrationRatio,
        double para0, double para1, double para2, double para3, double para4, double para5
        );
    double getImageWidth();
    double getImageHeight();
    double getiCx();
    double getiCy();
    ConfigData *getcd();

    void Rotate(Mat& src, Mat& dst, double angle);
    double AnyPoint(Mat &mapX, Mat &mapY, double alphaOffset, double betaOffset, double zoom, double magnification);
    double AnyPointM(float *mapX, float *mapY, int w, int h, double alphaOffset, double betaOffset, double zoom, double magnification);
    double AnyPointMM(float *mapX, float *mapY, int w, int h, double alphaOffset, double betaOffset, double zoom, double magnification);
    double AnyPointM2(float *mapX, float *mapY, int w, int h, double thetaX_degree, double thetaY_degree, double zoom, double magnification);
    double AnyPointMM2(float *mapX, float *mapY, int w, int h, double thetaX_degree, double thetaY_degree, double zoom, double magnification);
    double AnyPoint2_Remap(Mat& Src, Mat& Dest, double thetaX_degree, double thetaY_degree, double zoom, double magnification);
    double Panorama(Mat &mapX, Mat &mapY, double magnification /*, Mat &getAlpha, Mat &getBeta*/ );
    double revPanorama(Mat mapX, Mat mapY, Mat panoImage, Mat &result, double alpha_max, double beta_offset);
    double PanoEdge90(Mat &mapX, Mat &mapY, int alpha_width);
    double PanoramaM(float *mapX, float *mapY, int w, int h, double magnification, double alpha_max);
    double PanoramaM_Rt(float *mapX, float *mapY, int w, int h, double magnification, double alpha_max, double iC_alpha_degree, double iC_beta_degree);
    double Panorama(Mat &mapX, Mat &mapY, double maxAlpha, double minAlpha, double nowAlpha, Mat &getAlphaForAny, Mat &getBetaForAny, double magnification);
    double Remap(Mat &Src, Mat &Dest, Mat &mapX, Mat &mapY);
    int getRhoFromAlpha( double alpha );
    double getAlphaFromRho( int rho );
    // QPointF getAlphaBetaFromPos(int Mode, QPoint Pos);
    Point getAlphaBetaFromPos(int Mode, Point Pos);
    void oneFisheyePanorama(Mat &Src, Mat &Dest);
    void twoFisheyePanorama(Mat &Src1, Mat &Src2, Mat &Dest);
    int Max(int num1, int num2);
    Mat ImageRead(Mat image, string filename);
    void PixelOp(const Mat &src, Mat &dst);
    bool Blur(const Mat &src, Mat &dst);

};

#endif // MOILDEV_H
