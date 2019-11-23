#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>

#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QDesktopWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "moildev.h"
using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void camButtonClicked();
    void multiButtonClicked();
    void ch1ButtonClicked();
    void ch2ButtonClicked();
    void ch3ButtonClicked();
    void ch4ButtonClicked();
    void ch5ButtonClicked();
    void ch6ButtonClicked();
    void openCamera();
    void readFarme();
    void closeCamera();
    void takingPictures();

private:
        Moildev md ;
    Ui::MainWindow *ui;
        Mat image_input, image_input_s;
            Mat mapX[6], mapY[6];
    QRect screen ;
    QPushButton *m_button_cam, *m_button_multi, *m_button_ch[6] ;
    Mat image_display[6];
    cv::VideoCapture *cap0;
    QTimer    *timer;
    QImage    *imag;
    cv::Mat   *frame;
    bool CaptureState = false;
    void DisplayCh(int ch);
    void DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h);
    void Rotate(Mat& src, Mat& dst, double angle);
    void MatWrite(const string& filename, const Mat& mat);
    Mat MatRead(const string& filename);
    int x_base = 40;
    int y_base = 30;
    int ch_width = 400;
    int ch_height = 300;
};

#endif // MAINWINDOW_H
