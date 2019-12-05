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
#include <sys/types.h>
#include <dirent.h>
#include <json.hpp>
#include <ctime>
#include <sys/stat.h>


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
#include <QPoint>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "ui_mainwindow.h"
#include "moildev.h"
#include "mlabel.h"

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
    void openParameter();
    void on_actionLoad_Image_triggered();
    void on_actionExit_triggered();
    void on_actionMOIL_triggered();
    void on_actionLoad_triggered();
    void saveURL();

    void on_actionVideo_Source_triggered();
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onWheel(QWheelEvent *event);
    void snapshot();
    void onListItemClicked(QListWidgetItem* item);
    void upClicked();
    void downClicked();
    void leftClicked();
    void rightClicked();
    void resetClicked();
    void originalClicked(QMouseEvent*);
    void panoramaClicked();
    void anypointClicked();

private:
    void readImage(QString filename);
    void reSnapshotList();
    void openCamera();
    void closeCamera();
    void refreshMedicalState();

    QString m_sSettingsFile;
    QLabel* m_pLabel;
    QLineEdit* m_pEdit;
    QLabel* m_pMsg;
    QListWidget *listWidget;
    QHBoxLayout *layout;


    QPushButton* m_pButton;
    void loadSettings();
    void saveSettings();
    void closeEvent(QCloseEvent *event);


    QAction *openParaAction, *openImageAction, *snapAction,
    *upAction, *downAction, *leftAction, *rightAction;

    Moildev md ;
    Ui::MainWindow *ui;
    Mat image_input, image_input_s;
    Mat mapX[6], mapY[6];
    Mat mapX_Medi, mapY_Medi;
    Mat mapX_MediPano, mapY_MediPano;
    QRect screen ;
    QPushButton *m_button_cam, *m_button_multi, *m_button_ch[6];
    QPushButton *m_up, *m_down, *m_left, *m_right, *m_reset;
    QPushButton *m_paronama, *m_anypoint;
    Label *m_original;
    Mat image_display[6];
    cv::VideoCapture cap0;

    QPalette palWhite, palGray;

    QTimer    *timer;
    bool CaptureState = false;
    void showMoilInfo();
    void DisplayCh(int ch);
    void DisplayOne(bool refreshOriginal);
    void DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h);
    void Rotate(Mat& src, Mat& dst, double angle);
    void MatWrite(const string& filename, const Mat& mat);
    Mat MatRead(const string& filename);
    bool dbConnect(const QString &dbName);
    bool dbAddRecord(QString filename, int alpha, int beta, float zoom);
    bool dbGetRecord(QString filename, int *alpha, int *beta, float *zoom);
    int fix_width = 2592;
    int fix_height = 1944;
    double m_ratio = 1.0;
    int x_base = 40;
    int y_base = 30;
    int y_gap = 5;
    int ch_width = 400;
    int ch_height = 300;
    int main_width = 400;
    int main_height = 300;

    int DisplayMode = 0;
    int currCh = 0;
    int mouseState = 0 ;

    int currAlpha = 0;
    int currBeta = 0;
    double currZoom = 4;
    const double defaultZoom = 4;
    const double minZoom = 1;
    const double maxZoom = 12;

    int minAlpha = -90;
    int maxAlpha = 90;
    int minBeta = 0;
    int maxBeta = 360;

    int currInc = 3;

    QPoint prevMousePos, currMousePos ;

    std::string default_videoStreamURL = "http://192.168.100.2:8000/stream.mjpg";
    Label *pLabel;

    enum class MoilApp { CAR, MEDICAL };
    enum class MedicalState { ORIGINAL, ANYPOINT, PANORAMA };

    MoilApp MOIL_APP;
    MedicalState medicalState ;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
