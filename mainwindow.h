#pragma once
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
#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif
// #include "json.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <ctime>
#include <sys/stat.h>

#include <QtWidgets/QMainWindow>
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
#include <QToolTip>
#include <QTextEdit>
#include <QDate>
#include <QStandardPaths>
#include <QProgressDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDir>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtConcurrent/QtConcurrent>
#include "ui_mainwindow.h"
#include "moildev.h"
#include "mlabel.h"
#include "renderarea.h"

using namespace std;

// using json = nlohmann::json;

namespace Ui {
	class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();
private slots:

	void camButtonClicked();
	void recordButtonClicked();
	void multiButtonClicked();
	void ch1ButtonClicked();
	void ch2ButtonClicked();
	void ch3ButtonClicked();
	void ch4ButtonClicked();
	void ch5ButtonClicked();
	void ch6ButtonClicked();
	void readFrame();
	void openImageUi();
	void openVideoUi();
	void openParameterUi();
	void openNoteUi();
	void on_actionLoad_Image_triggered();
	void on_actionLoad_Video_triggered();
	void on_actionExit_triggered();
	void on_actionMOIL_triggered();
	void on_actionLoad_triggered();

	void on_actionVideo_Source_triggered();
	void onMousePressed(QMouseEvent *event);
	void onMouseReleased(QMouseEvent *event);
	void onDoubleClicked(QMouseEvent *event);
	void onMouseMoved(QMouseEvent *event);
	void onListMouseMoved(QMouseEvent*);
	void onWheeled(QWheelEvent *event);
	void onRenderareaDoubleClicked(QMouseEvent *event);
	void onRenderareaClicked(QMouseEvent *event);
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
	void radioButtonMode1Clicked();
	void radioButtonMode2Clicked();
	void on_horizontalSlider_actionTriggered(int action);
	void on_horizontalSlider_valueChanged(int value);
	void on_pushButton_play_clicked();
	void on_pushButton_left_clicked();
	void on_pushButton_right_clicked();
	void ListMenuRequested(const QPoint & pos);
    void httpFinished(QNetworkReply* reply);

	void on_pushButton_top_clicked();

	void on_ap_right_clicked();

	void on_ap_up_clicked();

	void on_ap_left_clicked();

	void on_ap_reset_clicked();

	void on_ap_down_clicked();

	void on_actionCamera_URL_triggered();

	void on_actionLoad_Note_triggered();

	void on_actionGenerate_Cache_triggered();

	void on_ap_DBG_F_clicked();

	void on_ap_DBG_L_clicked();

	void on_ap_DBG_R_clicked();

	void on_ap_DBG_D_clicked();

	void on_ap_DBG_LL_clicked();

	void on_ap_DBG_LR_clicked();

	void on_pushButton_Center_clicked();

private:
	Ui::MainWindow uim;
	Ui::MainWindow *ui = &uim;
	RenderArea *renderArea;
	static const int anyPoly_Max = 40;
	int anyPolyLength = 0;
	int anyPolyTopEdgeStart = 0;
	int anyPolyTopEdgeEnd = 0;
	QPointF anyPoly[anyPoly_Max];
	QPointF anyPolyCenter = QPointF(0, 0);
	QString m_sSettingsFile;
	QLabel* m_pLabel;
	QLineEdit* m_pEdit;
	QLabel* m_pMsg;
	QListWidget *listWidget;
	QHBoxLayout *layout;
	QMenu *sMenu;
	QAction *sAction;
	QString currMetaPath = "";
	QString currMetaDirName = "";
	QString currMetaFullPath = "";
	QPushButton* m_pButton;
	QAction *openParaAction, *openImageAction, *openVideoAction, *snapAction,
		*upAction, *downAction, *leftAction, *rightAction;
	Moildev md;
	string Para[2];
	float RadiusF[2];
	cv::Mat image_input, image_input_s;
	cv::Mat image_result;
	cv::Mat image_reverse;
	cv::Mat mapX[6], mapY[6];
	cv::Mat mapX_Medi, mapY_Medi;
	cv::Mat mapX_MediPano, mapY_MediPano;
	QRect screen;
	QPushButton *m_button_cam, *m_button_multi, *m_button_ch[6], *m_button_record;
	QPushButton *m_panorama, *m_anypoint;
	Label *m_original;
	cv::Mat image_display[6];
	cv::VideoCapture cap0;
	QPalette palWhite, palGray;
	QPixmap *pixmap;
	QTimer    *timer;
	bool CaptureState = false;
	// int fix_width = 2592;
	// int fix_height = 1944;
	int fix_width = 1920;
	int fix_height = 1080;
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
	int mouseState = 0;
	int currPara = 0;
	int currAlpha = 0;
	int currBeta = 0;
	double currZoom = 6;
	double defaultZoom = 6;
	double currPanoiCxAlpha = 0;
	double currPanoiCxBeta = 0;
	const double minZoom = 1;
	const double maxZoom = 12;
	int minAlpha = -90;
	int maxAlpha = 90;
	int minBeta = 0;
	int maxBeta = 360;
	int currInc = 3;
	int currFrame = 0;
	QPoint prevMousePos, currMousePos;
	std::string default_videoStreamURL = "http://192.168.100.2:8000/stream.mjpg";
	QString videoStreamURL = "";
	std::string default_cacheLocation = "/media/skc/WD4T"; // in settings.ini
	QString cacheLocation = "";
	QRadioButton *radioButtonMode1, *radioButtonMode2;
	QButtonGroup *buttonGroupMode;

	cv::VideoWriter videoWriter;
	Label *pLabel;
	enum class MediaType { NONE, IMAGE_FILE, CAMERA, VIDEO_FILE };
	enum class MoilApp { CAR, MEDICAL };
	enum class MedicalState { ORIGINAL, ANYPOINT, PANORAMA };
	enum class VideoSource { CAMERA, FILE };
	enum class PlayState { STOP, PLAY, PAUSE };
	enum class AnyPointState { MODE_1, MODE_2 };
	enum class CacheState { DISABLED, ENABLED };

	class AnyPointCacheInfo {
	public:
		int alpha;
		int beta;
		CacheState cacheState;
	};

	bool isCameraRecord = false;
	QString lastOpendFile = "";
	class VideoInfo {
	public:
		QString filename;
		VideoSource Source;
		int frame_width, frame_height, frame_count, avi_ratio, fps, pos_msec, pos_frame;
	};
	MediaType mediaType = MediaType::NONE;
	VideoInfo videoInfo;
	MoilApp MOIL_APP;
	MedicalState medicalState;
	// VideoSource videoSource ;
	PlayState playState;
	AnyPointState anypointState;
	AnyPointCacheInfo anypointCacheInfo;
    QNetworkAccessManager manager;

	QSqlDatabase db;
	QVBoxLayout *vbox_left, *vbox_right;
	QVBoxLayout *vbox_camera;
	QVBoxLayout *vbox_playCtrl;
	QHBoxLayout *hbox_viewCtrl;
	QHBoxLayout *hbox_time;
	QHBoxLayout *hbox_main;

	void initMat();
	void readImage(QString filename);
	void reSnapshotList();
	void openCamera();
	void closeCamera();
	void closeVideo();
	void refreshMedicalState();
	void calcAnypointPoly();
	void loadSettings();
	void saveSettings();
	void closeEvent(QCloseEvent *event);
	void openImage(QString filename);
	void openVideo(QString filename);
	void openParameter(QString filename);
	void loadParameterJson(string str);
	void openNote(QString filename);

	void showMoilInfo();
	void DisplayCh(int ch);
	void DisplayOne(bool refreshOriginal);
	void DisplayWindow(cv::Mat& src, QLabel *p_label, int x, int y, int w, int h);
	void Rotate(cv::Mat& src, cv::Mat& dst, double angle);
	void MatWrite(const string& filename, const cv::Mat& mat);
	cv::Mat MatRead(const string& filename);
	bool dbConnect(const QString &dbName);
	bool dbAddRecord(QString filename, int alpha, int beta, float zoom, int pos_frame, int vidType, QString vidSrc, int para_idx, QString comment);
	bool dbGetRecord(QString filename, int *alpha, int *beta, float *zoom, int *frame, int *vidType, QString *vidSrc, int *para_idx, QString *comment, bool changeState);
	bool dbDelRecord(QString filename);
	bool dbSetComment(QString filename, QString comment);
	void loadStyleSheet(const QString &sheetName);
	void delRecord(QString filename);
	void resizeEvent(QResizeEvent * e);
	void doAnyPoint();
    //void httpAnypoint(const QUrl &url);
    //QByteArray httpGetMatrix(const QUrl &url);
	void doRemap(cv::Mat &Src, cv::Mat &Dest, cv::Mat &mapX, cv::Mat &mapY);
	void doPanorama();
	void genAnypointCache(QString dirname, AnyPointState mode, int zoom, int alpha_max, int alpha_step, int beta_step);
	void fetchAnypointCache(AnyPointState mode, int alpha, int beta);
};
