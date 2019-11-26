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
#include <json.hpp>

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
#include <QFileDialog>
#include <QLineEdit>
#include <QSettings>
#include <QPalette>

#include "ui_mainwindow.h"
#include "moildev.h"
using namespace std;
using namespace cv;
using json = nlohmann::json;

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
    void readFrame();
    void openImage();
    void on_actionLoad_Image_triggered();
    void on_actionExit_triggered();
    void on_actionMOIL_triggered();
    void on_actionLoad_triggered();
    void saveURL();

    void on_actionVideo_Source_triggered();

private:
    void readImage(QString filename);
    void openCamera();
    void closeCamera();

    QString m_sSettingsFile;
    QLabel* m_pLabel;
    QLineEdit* m_pEdit;
    QPushButton* m_pButton;
    void loadSettings();
    void saveSettings();
    void closeEvent(QCloseEvent *event);

    QAction *openAction;

    Moildev md ;
    Ui::MainWindow *ui;
    Mat image_input, image_input_s;
    Mat mapX[6], mapY[6];
    QRect screen ;
    QPushButton *m_button_cam, *m_button_multi, *m_button_ch[6] ;
    Mat image_display[6];
    cv::VideoCapture cap0;

    QPalette palWhite, palGray;

    QTimer    *timer;
    bool CaptureState = false;
    void DisplayCh(int ch);
    void DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h);
    void Rotate(Mat& src, Mat& dst, double angle);
    void MatWrite(const string& filename, const Mat& mat);
    Mat MatRead(const string& filename);
    int fix_width = 2592;
    int fix_height = 1944;

    int x_base = 40;
    int y_base = 30;
    int y_gap = 5;
    int ch_width = 400;
    int ch_height = 300;
    int DisplayMode = 0;
    int currCh = 0 ;
    std::string default_videoStreamURL = "http://192.168.100.2:8000/stream.mjpg";
};

#endif // MAINWINDOW_H
