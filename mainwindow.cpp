#include "mainwindow.h"
#define CAR_MAP_CACHE_ENABLED false
#define DISCRETE_VIDEO_NOTE true
#define FORCE_CAMERA_USB true
#define DEBUG_MODE true
// 90.. 180, alpha degree of fisyeye camera
#define PANORAMA_ALPHA_MAX 110
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    MOIL_APP = MoilApp::MEDICAL;            // Options : CAR, MEDICAL
	medicalState = MedicalState::ANYPOINT;  // Options : ORIGINAL, ANYPOINT, PANORAMA
	anypointState = AnyPointState::MODE_2; // Options : MODE_1, MODE_2
	anypointCacheInfo.cacheState = CacheState::DISABLED; // Options : DISABLED, ENABLED

	db = QSqlDatabase::addDatabase("QSQLITE");
	if (!DISCRETE_VIDEO_NOTE) {
		// DIR* dir = opendir("camera_note");
		QDir qdir = QDir("camera_note");
		if (!qdir.exists())	
			qdir.mkdir("camera_note");		
		dbConnect("camera_note/moil.db");
	}

	screen = QApplication::desktop()->availableGeometry();
	timer = new QTimer(this);

	ui->setupUi(this);
	this->setWindowState(Qt::WindowMaximized);

	// this->move(0,screen.height()-this->geometry().height());
	this->setGeometry(QRect(QPoint(0, 0), QSize(screen.width(), screen.height())));

	// repo220_T2
/*
	md.Config("picamera", 1.4, 1.4,
		1320.0, 1017.0, 1.048,
		2592, 1944, 4.05,
		0, 0, 0, 0, -47.96, 222.86
				);
	md.Config("endoscope", 2, 2,
			1120.0, 520.0, 1,
			1920, 1080, 3,
			0, 0, 0, 0, 0, 130
				  );
	md.Config("t265_r", 3, 3,
			430.0, 397.0, 1,
			848, 800, 1.68,
			0, 0, 0, 0, -85.489, 288.65
				  );
*/
/*
		Para[0] = "{"
				  "\"cameraName\": \"picamera\","
				  "\"cameraSensorWidth\": 1.4,"
				  "\"cameraSensorHeight\": 1.4,"
				  "\"iCx\": 1298.0,"
				  "\"iCy\": 966.0,"
				  "\"ratio\": 1.048,"
				  "\"imageWidth\": 2592.0,"
				  "\"imageHeight\": 1944.0,"
				  "\"calibrationRatio\": 4.05,"
				  "\"parameter0\": 0,"
				  "\"parameter1\": 0,"
				  "\"parameter2\": 0,"
				  "\"parameter3\": 0,"
				  "\"parameter4\": -47.96,"
				  "\"parameter5\": 222.86"
				"}";
*/
	Para[0] = "{"
		"\"cameraName\": \"picamera\","
		"\"cameraSensorWidth\": 1.4,"
		"\"cameraSensorHeight\": 1.4,"
         "\"iCx\": 1298.0,"
         "\"iCy\": 966.0,"
		"\"ratio\": 1,"
		"\"imageWidth\": 2592.0,"
		"\"imageHeight\": 1944.0,"
        "\"calibrationRatio\":  3.4,"
		"\"parameter0\": 0,"
		"\"parameter1\": 0,"
		"\"parameter2\": 0,"
		"\"parameter3\": 10.11,"
		"\"parameter4\": -85.241,"
		"\"parameter5\": 282.21"
		"}";
	Para[1] = "{"
		"\"cameraName\": \"endoscope\","
		"\"cameraSensorWidth\": 2,"
		"\"cameraSensorHeight\": 2,"
		"\"iCx\": 1120.0,"
		"\"iCy\": 520.0,"
		"\"ratio\": 1,"
		"\"imageWidth\": 1920.0,"
		"\"imageHeight\": 1080.0,"
		"\"calibrationRatio\": 4.05,"
		"\"parameter0\": 0,"
		"\"parameter1\": 0,"
		"\"parameter2\": 0,"
		"\"parameter3\": 0,"
		"\"parameter4\": 0,"
		"\"parameter5\": 130"
		"}";
	RadiusF[0] = 0.5;
	RadiusF[1] = float(580) / 1080;
	// picamera 220 degree(iCx, iCy):
	// orig : (1320, 1017) -> (1278,958)
	// skc  : (1298, 966)
	// yu   : (1226, 966)
	currPara = int(MOIL_APP);
	loadParameterJson(Para[currPara]);
	fix_width = int(md.getImageWidth());
	fix_height = int(md.getImageHeight());

	CacheState oldCacheState = anypointCacheInfo.cacheState;
	anypointCacheInfo.cacheState = CacheState::DISABLED;
	initMat();
	anypointCacheInfo.cacheState = oldCacheState;

	// Benchmark :
	// intel i7-8700
	// resolution   : 2592 x 1944 = 5M pixels
	// md.AnyPoint(): 0.458 sec.  (_m): 0.498 sec.
	// md.Remap()   : 0.235 sec.
	// md.Remap()+Rotate() : 0.323 sec.
	// md.AnyPoint2_Remap() : 0.57 sec.
	// MatRad (X+Y) : 0.0157 sec. ( 1/1000 of AnyPoint)
	// panorama : 0.36 sec. -> 0.28 sec. without 2nd Mat
	// PanoramaM_Rt : 0.45 sec.
	// Mat File Reading: 20Mx2 -> (0.4 sec(non chahed) .. 0.01 sec.)
	//
	// clock_t tStart = clock();
	// ..
	// double time_clock = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	// time_t t0 = time(NULL);
	// ..
	// time_t t_diff = time(NULL) - t0;

	// default input image

	if (MOIL_APP == MoilApp::CAR)
		readImage("dataset/image.jpg");
	else if (MOIL_APP == MoilApp::MEDICAL) {
		if (!DISCRETE_VIDEO_NOTE)
			readImage("dataset/image2.jpg");
	}

	// USB camera
	// cap0 = cv::VideoCapture(0);

	// MJPG-Streamer
	// https://blog.miguelgrinberg.com/post/how-to-build-and-run-mjpg-streamer-on-the-raspberry-pi
	// https://github.com/jacksonliam/mjpg-streamer
	// const std::string videoStreamAddress = "http://192.168.100.2:8080/?action=stream";

	// python3
	// const std::string videoStreamAddress = "http://192.168.100.2:8000/stream.mjpg";
	// cap0.open(videoStreamAddress);
/*
	sMenu = new QMenu("Moil");
	sAction = new QAction("Delete");
	sMenu->addAction(sAction);
*/
	palWhite.setColor(QPalette::Button, QColor(Qt::white));
	palGray.setColor(QPalette::Button, QColor(Qt::lightGray));

	main_width = screen.width() - 590;
	main_height = screen.height() - 300;

	ch_width = (screen.width() - 120) / 3;
	ch_height = ch_width * 3 / 4;
	int ch_limit = (screen.height() - 120 - y_gap - 40) / 2;
	ch_height = (ch_height > ch_limit) ? ch_limit : ch_height;

	// original image
	// cv::resize(image_input, image_input_s, Size(ch_width,ch_height));
	if (FORCE_CAMERA_USB)
		m_button_cam = new QPushButton("USB Camera", this);
	else
		m_button_cam = new QPushButton("Camera", this);
	m_button_cam->setStyleSheet("border:3px solid darkgray;");

	connect(m_button_cam, SIGNAL(released()), this, SLOT(camButtonClicked()));

	m_button_record = new QPushButton("Record", this);
	m_button_record->setStyleSheet("border:3px solid darkgray;background-color:white");
	m_button_record->setMinimumHeight(30);
	connect(m_button_record, SIGNAL(released()), this, SLOT(recordButtonClicked()));
	ui->groupBox03->hide();

	switch (MOIL_APP) {
	case MoilApp::CAR:

		// ui->horizontalSlider->hide();
		m_button_record->hide();
		ui->listWidget->hide();
		// ui->groupBox01->hide();

		m_button_cam->setGeometry(QRect(QPoint(40, screen.height() - 130), QSize(100, 70)));

		m_button_multi = new QPushButton("Multi", this);
		m_button_multi->setGeometry(QRect(QPoint(160, screen.height() - 120), QSize(80, 50)));

		for (int i = 0; i < 6; i++) {
			char str[10];
			sprintf(str, "%d", i + 1);
			m_button_ch[i] = new QPushButton(str, this);
			m_button_ch[i]->setGeometry(QRect(QPoint(190 + (i + 1) * 60, screen.height() - 120), QSize(50, 50)));
		}

		ui->groupBox01->move(900, screen.height() - 210);
		ui->horizontalSlider->setStyleSheet("Height:30px;");
		ui->horizontalSlider->setMaximum(999);
		ui->horizontalSlider->setEnabled(false);
		ui->horizontalSlider->move(900, screen.height() - 155);
		ui->groupBox01->stackUnder(ui->horizontalSlider);


		connect(m_button_multi, SIGNAL(released()), this, SLOT(multiButtonClicked()));
		connect(m_button_ch[0], SIGNAL(released()), this, SLOT(ch1ButtonClicked()));
		connect(m_button_ch[1], SIGNAL(released()), this, SLOT(ch2ButtonClicked()));
		connect(m_button_ch[2], SIGNAL(released()), this, SLOT(ch3ButtonClicked()));
		connect(m_button_ch[3], SIGNAL(released()), this, SLOT(ch4ButtonClicked()));
		connect(m_button_ch[4], SIGNAL(released()), this, SLOT(ch5ButtonClicked()));
		connect(m_button_ch[5], SIGNAL(released()), this, SLOT(ch6ButtonClicked()));

		break;
	case MoilApp::MEDICAL:

		// QPoint arrowsPos(420, screen.height()-156);
		// QSize arrowSize(46,46);

		m_button_cam->setMinimumHeight(100);
		m_original = new Label("", this);
		m_original->setStyleSheet("border:3px solid darkgray;");
		connect(m_original, SIGNAL(clicked(QMouseEvent*)), this, SLOT(originalClicked(QMouseEvent*)));

		/*
			ui->groupBox03->resize(270,140);
			pixmap = new QPixmap(":/images/undo.svg");
			ui->ap_left->setIcon(QIcon(*pixmap));
			ui->ap_left->setIconSize(pixmap->rect().size());
			ui->ap_left->setStyleSheet("border:2px solid darkgray;");
			ui->ap_left->setShortcut(QKeySequence(Qt::Key_Left));

			pixmap = new QPixmap(":/images/redo.svg");
			ui->ap_right->setIcon(QIcon(*pixmap));
			ui->ap_right->setIconSize(pixmap->rect().size());
			ui->ap_right->setStyleSheet("border:2px solid darkgray;");
			ui->ap_right->setShortcut(QKeySequence(Qt::Key_Right));

			pixmap = new QPixmap(":/images/arrow-up2.svg");
			ui->ap_up->setIcon(QIcon(*pixmap));
			ui->ap_up->setIconSize(pixmap->rect().size());
			ui->ap_up->setStyleSheet("border:2px solid darkgray;");
			ui->ap_up->setShortcut(QKeySequence(Qt::Key_Up));

			pixmap = new QPixmap(":/images/arrow-down2.svg");
			ui->ap_down->setIcon(QIcon(*pixmap));
			ui->ap_down->setIconSize(pixmap->rect().size());
			ui->ap_down->setStyleSheet("border:2px solid darkgray;");
			ui->ap_down->setShortcut(QKeySequence(Qt::Key_Down));

			pixmap = new QPixmap(":/images/radio-checked2.svg");
			ui->ap_reset->setIcon(QIcon(*pixmap));
			ui->ap_reset->setIconSize(pixmap->rect().size());
			ui->ap_reset->setStyleSheet("border: none;");
			ui->ap_reset->setShortcut(QKeySequence(Qt::Key_Space));
		*/
		m_anypoint = new QPushButton("", this);
		m_anypoint->resize(150, 150);
		pixmap = new QPixmap(":/images/anypoint.png");
		m_anypoint->setIcon(QIcon(*pixmap));
		m_anypoint->setIconSize(QSize(150, 150));
		m_anypoint->setStyleSheet("border:3px solid darkgray;");
		m_anypoint->setShortcut(QKeySequence(Qt::Key_P));
		connect(m_anypoint, SIGNAL(released()), this, SLOT(anypointClicked()));

		m_panorama = new QPushButton("", this);
		m_panorama->resize(150, 150);
		pixmap = new QPixmap(":/images/pano2.png");
		m_panorama->setIcon(QIcon(*pixmap));
		m_panorama->setIconSize(QSize(150, 150));
		m_panorama->setStyleSheet("border:3px solid darkgray;");
		m_panorama->setShortcut(QKeySequence(Qt::Key_P));
		connect(m_panorama, SIGNAL(released()), this, SLOT(panoramaClicked()));

		ui->horizontalSlider->setStyleSheet("Height:30px;");
		ui->horizontalSlider->setMaximum(999);
		ui->horizontalSlider->setEnabled(false);

		pLabel = new Label("", this);
		pLabel->setMinimumSize(160, 120);
		pLabel->move(x_base, y_base);
		pLabel->stackUnder(m_button_cam);
		pLabel->setStyleSheet("border: none;");
		pLabel->setAutoFillBackground(true);
		QPalette pal = pLabel->palette();
		pal.setColor(QPalette::Window, QColor(Qt::black));
		// ui->label1->setPalette(pal);
		// ui->label1->setGeometry(pLabel->geometry());
		pLabel->setPalette(pal);
		pLabel->setStyleSheet("border:1px solid black;background-color:#181818");
		ui->label1->hide();

		connect(pLabel, SIGNAL(pressed(QMouseEvent*)), this, SLOT(onMousePressed(QMouseEvent*)));
		connect(pLabel, SIGNAL(clicked(QMouseEvent*)), this, SLOT(onMouseReleased(QMouseEvent*)));
		connect(pLabel, SIGNAL(doubled(QMouseEvent*)), this, SLOT(onDoubleClicked(QMouseEvent*)));
		connect(pLabel, SIGNAL(moved(QMouseEvent*)), this, SLOT(onMouseMoved(QMouseEvent*)));
		connect(pLabel, SIGNAL(wheeled(QWheelEvent*)), this, SLOT(onWheeled(QWheelEvent*)));

		connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
			this, SLOT(onListItemClicked(QListWidgetItem*)));
		ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ListMenuRequested(const QPoint &)));
		// connect(ui->listWidget, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(onListMouseMoved(QMouseEvent*)));


	 // ui->listWidget->setToolTip("Image List");
		ui->listWidget->setIconSize(QSize(240, 180));
		ui->listWidget->setViewMode(QListView::IconMode);
		pLabel->stackUnder(ui->listWidget);
		// reSnapshotList();


		break;
	}
	connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));

	QToolBar *toolBar = addToolBar(tr("&File"));
	openParaAction = new QAction(QIcon(":/images/file-text2.svg"), tr("&Load Parameter"), this);
	toolBar->addAction(openParaAction);
	connect(openParaAction, SIGNAL(triggered()), this, SLOT(openParameterUi()));

	openImageAction = new QAction(QIcon(":/images/image.svg"), tr("&Load Image"), this);
	toolBar->addAction(openImageAction);
	connect(openImageAction, SIGNAL(triggered()), this, SLOT(openImageUi()));

	openVideoAction = new QAction(QIcon(":/images/film.svg"), tr("&Load Video"), this);
	toolBar->addAction(openVideoAction);
	connect(openVideoAction, SIGNAL(triggered()), this, SLOT(openVideoUi()));
	// toolBar->addWidget(buttonGroupMode);
	QToolBar *toolBar2 = addToolBar(tr("&Bar2"));
	snapAction = new QAction(QIcon(":/images/camera.svg"), tr("&Take Snapshot"), this);
	QWidget *spacerWidget = new QWidget(this);
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);
	toolBar2->addWidget(spacerWidget);
	toolBar2->addAction(snapAction);
	connect(snapAction, SIGNAL(triggered()), this, SLOT(snapshot()));

	radioButtonMode1 = new QRadioButton("Mode 1", this);
	radioButtonMode2 = new QRadioButton("Mode 2", this);
	buttonGroupMode = new QButtonGroup(this);
	buttonGroupMode->addButton(radioButtonMode1, 0);
	buttonGroupMode->addButton(radioButtonMode2, 1);
	if (anypointState == AnyPointState::MODE_1)
		radioButtonMode1->setChecked(true);
	else
		radioButtonMode2->setChecked(true);
	radioButtonMode1->move(200, 25);
	radioButtonMode2->move(280, 25);
	connect(radioButtonMode1, SIGNAL(clicked(bool)),
		this, SLOT(radioButtonMode1Clicked()));
	connect(radioButtonMode2, SIGNAL(clicked(bool)),
		this, SLOT(radioButtonMode2Clicked()));

	m_pLabel = new QLabel("Camera URL :", this);
	m_pLabel->setGeometry(300, 25, 100, 30);
	m_pEdit = new QLineEdit("", this);
	m_pEdit->setGeometry(400, 25, 400, 30);

	m_pMsg = new QLabel("", this);
	m_pMsg->setGeometry(600, 25, 800, 30);

	m_pLabel->hide();
	m_pEdit->hide();

	pixmap = new QPixmap(":/images/play3.svg");
	ui->pushButton_play->setIcon(QIcon(*pixmap));
	ui->pushButton_play->setIconSize(pixmap->rect().size());
	ui->pushButton_play->setStyleSheet("border:2px solid darkgray;");
	ui->pushButton_play->setShortcut(QKeySequence(Qt::Key_Space));

	pixmap = new QPixmap(":/images/first.svg");
	ui->pushButton_top->setIcon(QIcon(*pixmap));
	ui->pushButton_top->setIconSize(pixmap->rect().size());
	ui->pushButton_top->setStyleSheet("border:2px solid darkgray;");

	pixmap = new QPixmap(":/images/previous2.svg");
	ui->pushButton_left->setIcon(QIcon(*pixmap));
	ui->pushButton_left->setIconSize(pixmap->rect().size());
	ui->pushButton_left->setStyleSheet("border:2px solid darkgray;");

	pixmap = new QPixmap(":/images/next2.svg");
	ui->pushButton_right->setIcon(QIcon(*pixmap));
	ui->pushButton_right->setIconSize(pixmap->rect().size());
	ui->pushButton_right->setStyleSheet("border:2px solid darkgray;");
	if (!DEBUG_MODE) {
		ui->actionGenerate_Cache->setVisible(false);
	}
	m_sSettingsFile = QApplication::applicationDirPath() + "/settings.ini";

	if (MOIL_APP == MoilApp::MEDICAL) {
		// renderArea = new RenderArea(ui->groupBox03);
		renderArea = new RenderArea(m_original);
		// renderArea->resize( QSize( 140, 140));
		renderArea->resize(QSize(400, 280));
		// renderArea->move(125,0);
		renderArea->setSize(int(100 / sqrt(currZoom)));
		Qt::PenStyle style = Qt::PenStyle(1);
		Qt::PenCapStyle cap = Qt::PenCapStyle(1);
		Qt::PenJoinStyle join = Qt::PenJoinStyle(1);
		renderArea->setPen(QPen(Qt::red, 2, style, cap, join));
		renderArea->setBrush(QBrush(Qt::green, Qt::LinearGradientPattern));
		renderArea->setStyleSheet("border: none;");
		connect(renderArea, SIGNAL(clicked(QMouseEvent*)), this, SLOT(onRenderareaClicked(QMouseEvent*)));
		connect(renderArea, SIGNAL(wheeled(QWheelEvent*)), this, SLOT(onWheeled(QWheelEvent*)));
		connect(renderArea, SIGNAL(doubleclicked(QMouseEvent*)), this, SLOT(onRenderareaDoubleClicked(QMouseEvent*)));

		refreshMedicalState();

		vbox_left = new QVBoxLayout();
		vbox_right = new QVBoxLayout();
		vbox_camera = new QVBoxLayout();
		vbox_playCtrl = new QVBoxLayout();
		hbox_viewCtrl = new QHBoxLayout();
		hbox_time = new QHBoxLayout();
		hbox_main = new QHBoxLayout();

		// m_button_cam->setFixedHeight(140);
		// m_original->setFixedWidth(400);
		// m_original->setFixedHeight(280);
		// ui->groupBox03->setFixedWidth(400);
		// ui->listWidget->setFixedWidth(400);

		hbox_time->addWidget(ui->time1, 1);
		hbox_time->addStretch();
		hbox_time->addWidget(ui->time2, 0, Qt::AlignRight);

		vbox_playCtrl->addWidget(ui->groupBox01, 3);
		vbox_playCtrl->addLayout(hbox_time, 1);
		vbox_playCtrl->addWidget(ui->horizontalSlider, 1);
		vbox_playCtrl->addStretch();

		vbox_camera->addWidget(m_button_cam, 5);
		vbox_camera->addWidget(m_button_record, 1);

		hbox_viewCtrl->addLayout(vbox_camera, 2);
		// hbox_viewCtrl->addWidget(m_original, 4);
		// hbox_viewCtrl->addWidget(ui->groupBox03, 5);

		if (DEBUG_MODE) {
			hbox_viewCtrl->addWidget(ui->groupBox04, 2);
		}
		else
			ui->groupBox04->hide();

		hbox_viewCtrl->addWidget(m_anypoint, 2);
		hbox_viewCtrl->addWidget(m_panorama, 2);
		hbox_viewCtrl->addLayout(vbox_playCtrl, 12);

		vbox_left->addWidget(pLabel, 8);
		pLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		vbox_left->addStretch();
		vbox_left->addLayout(hbox_viewCtrl, 1);

		vbox_right->addWidget(ui->listWidget, 3);
		vbox_right->addStretch();
		vbox_right->addWidget(m_original, 2);

		hbox_main->addLayout(vbox_left, 9);
		hbox_main->addStretch();
		// hbox_main->addWidget(ui->listWidget,2);
		hbox_main->addLayout(vbox_right, 4);

		this->centralWidget()->setLayout(hbox_main);
	}
	else {
		ui->time1->move(ui->horizontalSlider->x(), ui->horizontalSlider->y() - 5);
		ui->time2->move(ui->horizontalSlider->x() + ui->horizontalSlider->width() - 30, ui->horizontalSlider->y() - 5);
		ui->groupBox04->hide();
	}

	loadSettings();
	loadStyleSheet("mainmoil");
	switch (MOIL_APP) {
	case MoilApp::CAR:
		DisplayCh(0);
		break;
	case MoilApp::MEDICAL:
		renderArea->setCenter(QPointF(md.getiCx() / md.getImageWidth(), md.getiCy() / md.getImageHeight()), RadiusF[currPara]);
		renderArea->setAnypointPoly(anyPoly, anyPolyLength, anyPolyTopEdgeStart, anyPolyTopEdgeEnd, anyPolyCenter);
		DisplayOne(true);
	}

	if (!DEBUG_MODE) {
		radioButtonMode1->hide();
		radioButtonMode2->hide();
		m_pMsg->hide();
	}
	else {
		if (MOIL_APP == MoilApp::MEDICAL) {
			pLabel->isDrawCenterEnabled = true;
			renderArea->isDrawCenterEnabled = true;
		}
	}

	// Benchmark :
/*
		clock_t tStart = clock();
		for (int i= 0 ;i< 10;i++)
			md.AnyPoint2_Remap(image_input, image_result, (double)currAlpha, (double)currBeta, currZoom, m_ratio );
		double time_clock0 = (double)(clock() - tStart)/CLOCKS_PER_SEC ;
		qDebug( "%f", time_clock0 );
*/
}

void MainWindow::initMat()
{
	double w = fix_width;
	double h = fix_height;
	double calibrationWidth = md.getImageWidth();
    image_result = Mat(h, w, CV_8UC3);

	mapX_Medi = Mat(h, w, CV_32F);
	mapY_Medi = Mat(h, w, CV_32F);

	mapX_MediPano = Mat(h, w, CV_32F);
	mapY_MediPano = Mat(h, w, CV_32F);
	image_reverse.create(h, w, CV_8UC3);

	if (anypointState == AnyPointState::MODE_1) {
		mapX[0] = Mat(h, w, CV_32F);
		mapY[0] = Mat(h, w, CV_32F);
		mapX[1] = Mat(w, h, CV_32F);
		mapX[2] = Mat(w, h, CV_32F);
		mapY[1] = Mat(w, h, CV_32F);
		mapY[2] = Mat(w, h, CV_32F);
		for (uint i = 3; i < 6; i++)
			mapX[i] = Mat(h, w, CV_32F);
		for (uint i = 3; i < 6; i++)
			mapY[i] = Mat(h, w, CV_32F);

	}
	else { // MODE_2
		for (uint i = 0; i < 6; i++)
			mapX[i] = Mat(h, w, CV_32F);
		for (uint i = 0; i < 6; i++)
			mapY[i] = Mat(h, w, CV_32F);
	}


	m_ratio = w / calibrationWidth;

	char str_x[12], str_y[12];
	int i = 0;

	if (MOIL_APP == MoilApp::CAR)
	{
		if (CAR_MAP_CACHE_ENABLED) {

			bool map_exist = true;

			while (map_exist && (i < 6)) {
				sprintf(str_x, "matX%d", i); sprintf(str_y, "matY%d", i);
				if (!fopen(str_x, "r") || !fopen(str_y, "r"))
					map_exist = false;
				i++;
			}
			if (map_exist) {
				clock_t tStart = clock();
				for (i = 0; i < 6; i++) {
					sprintf(str_x, "matX%d", i); sprintf(str_y, "matY%d", i);
					mapX[i] = MatRead(str_x);
					mapY[i] = MatRead(str_y);
				}

				double time_clock = (double)(clock() - tStart) / CLOCKS_PER_SEC;
				qDebug("Reading Time : %f", time_clock);
			}
			else {
				if (anypointState == AnyPointState::MODE_1) {
					md.AnyPointM((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 4, m_ratio);       // front view
					md.AnyPointM((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 70, 270, 4, m_ratio);   // left view, rotate 90
					md.AnyPointM((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 70, 90, 4, m_ratio);    // right view, rotate -90
					md.AnyPointM((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 4, m_ratio);      // Down view ( zoom: 2/4 )
					md.AnyPointM((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, 70, 225, 4, m_ratio);   // left-lower view, rotate 180
					md.AnyPointM((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, 70, 135, 4, m_ratio);   // right-lower view, rotate 180
				}
				else {
					md.AnyPointM2((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 6, m_ratio);       // front view
					md.AnyPointM2((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 0, -70, 6, m_ratio);   // left view
					md.AnyPointM2((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 0, 70, 6, m_ratio);    // right view
					md.AnyPointM2((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 6, m_ratio);      // Down view
					md.AnyPointM2((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, -45, -45, 6, m_ratio);   // left-lower view
					md.AnyPointM2((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, -45, 45, 6, m_ratio);   // right-lower view
				}
				for (i = 0; i < 6; i++) {
					sprintf(str_x, "matX%d", i); sprintf(str_y, "matY%d", i);
					MatWrite(str_x, mapX[i]);
					MatWrite(str_y, mapY[i]);
				}
			}

		}
		else {
			if (anypointState == AnyPointState::MODE_1) {
				md.AnyPointM((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 4, m_ratio);       // front view
				md.AnyPointM((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 70, 270, 4, m_ratio);   // left view, rotate 90
				md.AnyPointM((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 70, 90, 4, m_ratio);    // right view, rotate -90
				md.AnyPointM((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 4, m_ratio);      // Down view ( zoom: 2/4 )
				md.AnyPointM((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, 70, 225, 4, m_ratio);   // left-lower view, rotate 180
				md.AnyPointM((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, 70, 135, 4, m_ratio);   // right-lower view, rotate 180
			}
			else {
				md.AnyPointM2((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 6, m_ratio);       // front view
				md.AnyPointM2((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 0, -70, 6, m_ratio);   // left view
				md.AnyPointM2((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 0, 70, 6, m_ratio);    // right view
				md.AnyPointM2((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 6, m_ratio);      // Down view
				md.AnyPointM2((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, -45, -45, 6, m_ratio);   // left-lower view
				md.AnyPointM2((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, -45, 45, 6, m_ratio);   // right-lower view
			}
		}
	}
	else // ( MOIL_APP == MoilApp::MEDICAL )
	{
		doAnyPoint();
		md.PanoEdge90(mapX_MediPano, mapY_MediPano, 1);
		doPanorama();
	}

}
void MainWindow::showMoilInfo()
{
	QString str;
	if (MOIL_APP == MoilApp::MEDICAL)
	{
		if (medicalState == MedicalState::ANYPOINT) {
			if (anypointState == AnyPointState::MODE_1) {
				str.sprintf("Alpha: %d   Beta: %d   Zoom: %.2f   Pitch: %d   Yaw: 0   Roll: %d", currAlpha, currBeta, currZoom, currAlpha, currBeta);
			}
			else { // MODE_2
				str.sprintf("Alpha: %d   Beta: %d   Zoom: %.2f   Pitch: %d   Yaw: %d   Roll: 0", currAlpha, currBeta, currZoom, currAlpha, currBeta);
			}
		}
		else
			str = "";
		if (m_pMsg != NULL)
			m_pMsg->setText(str);
		ui->lineEdit_Cx->setText(QString::number(md.getiCx(), 'd', 0));
		ui->lineEdit_Cy->setText(QString::number(md.getiCy(), 'd', 0));
	}
}

void MainWindow::readImage(QString filename)
{
	std::string fname = filename.toUtf8().constData();
    image_input = imread(fname, IMREAD_COLOR);
	if (!image_input.empty())
	{

		if (image_input.cols == 2592 && image_input.rows == 1944 && currPara != 0) {
			currPara = 0;
			loadParameterJson(Para[currPara]);
			fix_width = md.getImageWidth();
			fix_height = md.getImageHeight();
			initMat();
		}
		else if (image_input.cols == 1920 && (image_input.rows == 1080 || image_input.rows == 1088) && currPara != 1) {
			currPara = 1;
			loadParameterJson(Para[currPara]);
			fix_width = md.getImageWidth();
			fix_height = md.getImageHeight();
			initMat();
		}

		if ((image_input.cols != fix_width) || (image_input.rows != fix_height)) {
			cv::resize(image_input, image_input, Size(fix_width, fix_height));
		}
		Vec3b p(0, 0, 0);
		image_input.at<Vec3b>(0, 1) = p;
	}
}

void MainWindow::snapshot()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	char f_name[128];
	char path[128];
	char filename[128];

	if (cap0.isOpened() && (mediaType == MediaType::CAMERA)) {

	}
	else if ((!cap0.isOpened()) || (mediaType != MediaType::VIDEO_FILE)) {
		QMessageBox msgBox;
		msgBox.setText("Please open a video file.\n\n");
		msgBox.exec();
		return;
	}
	sprintf(f_name, "%04d%02d%02d_%02d%02d%02d.jpg", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
	strcpy(&path[0], currMetaFullPath.toLocal8Bit().data());
	strcpy(&filename[0], &path[0]);
	strcat(&filename[0], "/");
	strcat(&filename[0], &f_name[0]);

	QDir qdir = QDir(currMetaFullPath);
	if (!qdir.exists())
		qdir.mkdir(currMetaFullPath);

	switch (MOIL_APP) {
	case MoilApp::CAR:
		Mat *thisMat;
		switch (currCh) {
		case 2:
			thisMat = &image_display[0];
			break;
		case 1:
			thisMat = &image_display[1];
			break;
		case 3: case 4: case 5: case 6:
			thisMat = &image_display[currCh - 1];
			break;
		}
		if (!thisMat->empty()) {
			Mat image_display_tmp;
			cvtColor(*thisMat, image_display_tmp, COLOR_RGB2BGR);
			cv::imwrite(filename, image_display_tmp);
			reSnapshotList();
		}
		break;
	case MoilApp::MEDICAL:

		if (!image_display[0].empty()) {

			// fisheye image
			Mat image_display_tmp;
			cv::imwrite(filename, image_input);

			// thumbnail
			strcpy(&path[0], currMetaFullPath.toLocal8Bit().data());
			strcat(&path[0], "/thumbnail");
			strcpy(&filename[0], &path[0]);
			strcat(&filename[0], "/");
			strcat(&filename[0], &f_name[0]);
			QDir qdir = QDir(currMetaFullPath + "/thumbnail");
			if (!qdir.exists())
				qdir.mkdir(currMetaFullPath + "/thumbnail");

			cvtColor(image_display[0], image_display_tmp, COLOR_RGB2BGR);
			cv::imwrite(filename, image_display_tmp);
			QString str(f_name);
			QString srcFilename;

			if (cap0.isOpened()) {
				switch (mediaType) {
				case MediaType::CAMERA:
					srcFilename = "";
					break;
				case MediaType::VIDEO_FILE:
					srcFilename = videoInfo.filename;
					break;
				}
			}

			dbAddRecord(str, currAlpha, currBeta, currZoom, videoInfo.pos_frame, (int)mediaType, srcFilename, currPara, "Moil Comment");
			reSnapshotList();
		}
		break;
	}

}

void MainWindow::reSnapshotList()
{
	dirent *dp;
	DIR *dir = opendir(currMetaFullPath.toLocal8Bit().data());
	/*
			if( !cap0.isOpened() || ( mediaType != MediaType::VIDEO_FILE ) && ( mediaType != MediaType::CAMERA ) ) {
				ui->listWidget->clear();
				return ;
			}
	*/

	if (dir) {
		ui->listWidget->clear();
		while ((dp = readdir(dir)) != NULL)
		{
			QString str(dp->d_name);
			if (str.toLower().contains(".jpg"))
			{
				QListWidgetItem *wi = new QListWidgetItem();
				wi->setIcon(QIcon(currMetaFullPath + "/thumbnail/" + str));
				wi->setText(str);
				// wi->setToolTip(str);

				int a, b, pos_frame, vt, p;
				float z;
				QString vidsrc;
				QString comment;

				bool Ret = dbGetRecord(str, &a, &b, &z, &pos_frame, &vt, &vidsrc, &p, &comment, false);
				if (Ret)
					wi->setToolTip(comment);

				ui->listWidget->addItem(wi);
				qDebug("%s", dp->d_name);
			}
		}
		ui->listWidget->sortItems();
		closedir(dir);
	}
}
void MainWindow::DisplayCh(int ch)
{
	Mat image_resultv;
	if (image_input.empty()) return;

	if (anypointState == AnyPointState::MODE_1) {
		switch (ch) {
		case 0:  // 2 x 3
			doRemap(image_input, image_result, mapX[0], mapY[0]);
			cv::resize(image_result, image_display[0], Size(ch_width, ch_height));
			cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
			DisplayWindow(image_display[0], ui->label2, x_base + ch_width, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_resultv, mapX[1], mapY[1]);
			Rotate(image_resultv, image_result, 90.0);
			cv::resize(image_result, image_display[1], Size(ch_width, ch_height));
			cvtColor(image_display[1], image_display[1], COLOR_BGR2RGB);
			DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_resultv, mapX[2], mapY[2]);
			Rotate(image_resultv, image_result, -90.0);
			cv::resize(image_result, image_display[2], Size(ch_width, ch_height));
			cvtColor(image_display[2], image_display[2], COLOR_BGR2RGB);
			DisplayWindow(image_display[2], ui->label3, x_base + ch_width * 2, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[3], mapY[3]);
			cv::resize(image_result, image_display[3], Size(ch_width, ch_height));
			cvtColor(image_display[3], image_display[3], COLOR_BGR2RGB);
			DisplayWindow(image_display[3], ui->label4, x_base + ch_width, ch_height + y_gap, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[4], mapY[4]);
			md.Rotate(image_result, image_result, 180.0);
			cv::resize(image_result, image_display[4], Size(ch_width, ch_height));
			cvtColor(image_display[4], image_display[4], COLOR_BGR2RGB);
			DisplayWindow(image_display[4], ui->label5, x_base, ch_height + y_gap, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[5], mapY[5]);
			md.Rotate(image_result, image_result, 180.0);
			cv::resize(image_result, image_display[5], Size(ch_width, ch_height));
			cvtColor(image_display[5], image_display[5], COLOR_BGR2RGB);
			DisplayWindow(image_display[5], ui->label6, x_base + ch_width * 2, ch_height + y_gap, ch_width, ch_height - y_base);
			break;
		case 2:
			doRemap(image_input, image_result, mapX[0], mapY[0]);
			cv::resize(image_result, image_display[0], Size(ch_width * 3 - 60, ch_height * 2 - 45));
			cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
			DisplayWindow(image_display[0], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 1:
			doRemap(image_input, image_resultv, mapX[1], mapY[1]);
			Rotate(image_resultv, image_result, 90.0);
			cv::resize(image_result, image_display[1], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[1], image_display[1], COLOR_BGR2RGB);
			DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 3:
			doRemap(image_input, image_resultv, mapX[2], mapY[2]);
			Rotate(image_resultv, image_result, -90.0);
			cv::resize(image_result, image_display[2], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[2], image_display[2], COLOR_BGR2RGB);
			DisplayWindow(image_display[2], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 4:
			doRemap(image_input, image_result, mapX[3], mapY[3]);
			cv::resize(image_result, image_display[3], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[3], image_display[3], COLOR_BGR2RGB);
			DisplayWindow(image_display[3], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 5:
			doRemap(image_input, image_result, mapX[4], mapY[4]);
			md.Rotate(image_result, image_result, 180.0);
			cv::resize(image_result, image_display[4], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[4], image_display[4], COLOR_BGR2RGB);
			DisplayWindow(image_display[4], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 6:
			doRemap(image_input, image_result, mapX[5], mapY[5]);
			md.Rotate(image_result, image_result, 180.0);
			cv::resize(image_result, image_display[5], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[5], image_display[5], COLOR_BGR2RGB);
			DisplayWindow(image_display[5], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		}
	}
	else { // Mode 2
		switch (ch) {
		case 0:  // 2 x 3
			doRemap(image_input, image_result, mapX[0], mapY[0]);
                        cv::resize(image_result, image_display[0], Size(ch_width, ch_height),0,0,INTER_LINEAR);
			cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
			DisplayWindow(image_display[0], ui->label2, x_base + ch_width, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[1], mapY[1]);
			cv::resize(image_result, image_display[1], Size(ch_width, ch_height));
			cvtColor(image_display[1], image_display[1], COLOR_BGR2RGB);
			DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[2], mapY[2]);
			cv::resize(image_result, image_display[2], Size(ch_width, ch_height));
			cvtColor(image_display[2], image_display[2], COLOR_BGR2RGB);
			DisplayWindow(image_display[2], ui->label3, x_base + ch_width * 2, y_base, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[3], mapY[3]);
			cv::resize(image_result, image_display[3], Size(ch_width, ch_height));
			cvtColor(image_display[3], image_display[3], COLOR_BGR2RGB);
			DisplayWindow(image_display[3], ui->label4, x_base + ch_width, ch_height + y_gap, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[4], mapY[4]);
			cv::resize(image_result, image_display[4], Size(ch_width, ch_height));
			cvtColor(image_display[4], image_display[4], COLOR_BGR2RGB);
			DisplayWindow(image_display[4], ui->label5, x_base, ch_height + y_gap, ch_width, ch_height - y_base);

			doRemap(image_input, image_result, mapX[5], mapY[5]);
			cv::resize(image_result, image_display[5], Size(ch_width, ch_height));
			cvtColor(image_display[5], image_display[5], COLOR_BGR2RGB);
			DisplayWindow(image_display[5], ui->label6, x_base + ch_width * 2, ch_height + y_gap, ch_width, ch_height - y_base);
			break;
		case 2:
			doRemap(image_input, image_result, mapX[0], mapY[0]);
			cv::resize(image_result, image_display[0], Size(ch_width * 3 - 60, ch_height * 2 - 45));
			cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
			DisplayWindow(image_display[0], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 1:
			doRemap(image_input, image_result, mapX[1], mapY[1]);
			cv::resize(image_result, image_display[1], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[1], image_display[1], COLOR_BGR2RGB);
			DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 3:
			doRemap(image_input, image_result, mapX[2], mapY[2]);
			cv::resize(image_result, image_display[2], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[2], image_display[2], COLOR_BGR2RGB);
			DisplayWindow(image_display[2], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 4:
			doRemap(image_input, image_result, mapX[3], mapY[3]);
			cv::resize(image_result, image_display[3], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[3], image_display[3], COLOR_BGR2RGB);
			DisplayWindow(image_display[3], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 5:
			doRemap(image_input, image_result, mapX[4], mapY[4]);
			cv::resize(image_result, image_display[4], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[4], image_display[4], COLOR_BGR2RGB);
			DisplayWindow(image_display[4], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		case 6:
			doRemap(image_input, image_result, mapX[5], mapY[5]);
			cv::resize(image_result, image_display[5], Size(ch_width * 3 * 0.9, ch_height * 2 * 0.9));
			cvtColor(image_display[5], image_display[5], COLOR_BGR2RGB);
			DisplayWindow(image_display[5], ui->label1, x_base, y_base, ch_width * 3 * 0.9, ch_height * 2 * 0.9);
			break;
		}

	}
	if (ch == 0) {
		ui->label2->setVisible(true);
		ui->label3->setVisible(true);
		ui->label4->setVisible(true);
		ui->label5->setVisible(true);
		ui->label6->setVisible(true);
		snapAction->setVisible(false);
	}
	else
	{
		ui->label2->setVisible(false);
		ui->label3->setVisible(false);
		ui->label4->setVisible(false);
		ui->label5->setVisible(false);
		ui->label6->setVisible(false);
		snapAction->setVisible(true);
	}

	m_button_multi->setPalette(palWhite);
	for (int i = 0; i < 6; i++)
		m_button_ch[i]->setPalette(palWhite);
	if (ch == 0)
		m_button_multi->setPalette(palGray);
	else
		m_button_ch[ch - 1]->setPalette(palGray);

	currCh = ch;

}

void MainWindow::DisplayOne(bool refreshOriginal)
{
	if (image_input.empty()) {
		// pLabel->setGeometry(x_base, y_base, main_width, main_height );
		return;
	}
	Mat image_original;
	// qDebug() << pLabel->width();

	if (refreshOriginal) {
		cv::resize(image_input, image_original, Size(m_original->geometry().width() - 2, m_original->geometry().height() - 2));
		cvtColor(image_original, image_original, COLOR_BGR2RGB);
		DisplayWindow(image_original, m_original, m_original->geometry().x(), m_original->geometry().y(), m_original->geometry().width() - 2, m_original->geometry().height() - 2);
	}

	switch (medicalState) {
	case MedicalState::ORIGINAL:
		cv::resize(image_input, image_display[0], Size(main_width, main_height));
		break;
	case MedicalState::ANYPOINT:
		calcAnypointPoly();
		renderArea->setAnypointPoly(anyPoly, anyPolyLength, anyPolyTopEdgeStart, anyPolyTopEdgeEnd, anyPolyCenter);
		doRemap(image_input, image_result, mapX_Medi, mapY_Medi);
		cv::resize(image_result, image_display[0], Size(main_width, main_height));
		cv::resize(image_result, image_result, Size(main_width, main_height));
		renderArea->setPos(currAlpha, currBeta);
		break;
	case MedicalState::PANORAMA:
		renderArea->setPanorama();
		doRemap(image_input, image_result, mapX_MediPano, mapY_MediPano);
		cv::resize(image_result, image_display[0], Size(main_width, main_height));
		break;

	}
	cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
	DisplayWindow(image_display[0], pLabel, x_base, y_base, main_width, main_height);
	showMoilInfo();
}


void MainWindow::DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h)
{
	QImage q_image1 = QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
	p_label->setPixmap(QPixmap::fromImage(q_image1));
	p_label->setStyleSheet("border:1px solid black;");
	if (MOIL_APP != MoilApp::MEDICAL)
		p_label->setGeometry(x, y, w, h);
}

void MainWindow::Rotate(Mat& src, Mat& dst, double angle)
{
	Point2f center((src.cols - 1) / 2.0, (src.rows - 1) / 2.0);
	Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
	Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();
	rot.at<double>(0, 2) += bbox.width / 2.0 - src.cols / 2.0;
	rot.at<double>(1, 2) += bbox.height / 2.0 - src.rows / 2.0;
	warpAffine(src, dst, rot, bbox.size());
}

void MainWindow::MatWrite(const string& filename, const Mat& mat)
{
	ofstream fs(filename, fstream::binary);

	// Header
	int type = mat.type();
	int channels = mat.channels();
	fs.write((char*)&mat.rows, sizeof(int));    // rows
	fs.write((char*)&mat.cols, sizeof(int));    // cols
	fs.write((char*)&type, sizeof(int));        // type
	fs.write((char*)&channels, sizeof(int));    // channels

	// Data
	if (mat.isContinuous())
	{
		fs.write(mat.ptr<char>(0), (mat.dataend - mat.datastart));
	}
	else
	{
		int rowsz = CV_ELEM_SIZE(type) * mat.cols;
		for (int r = 0; r < mat.rows; ++r)
		{
			fs.write(mat.ptr<char>(r), rowsz);
		}
	}
}

Mat MainWindow::MatRead(const string& filename)
{
	ifstream fs(filename, fstream::binary);

	// Header
	int rows, cols, type, channels;
	fs.read((char*)&rows, sizeof(int));         // rows
	fs.read((char*)&cols, sizeof(int));         // cols
	fs.read((char*)&type, sizeof(int));         // type
	fs.read((char*)&channels, sizeof(int));     // channels

	// Data
	Mat mat(rows, cols, type);
	fs.read((char*)mat.data, CV_ELEM_SIZE(type) * rows * cols);

	return mat;
}

void MainWindow::camButtonClicked()
{
	if (!cap0.isOpened()) {
		openCamera();
	}
	else {
		closeCamera();
	}
}

void MainWindow::recordButtonClicked()
{

	if (cap0.isOpened() && mediaType == MediaType::CAMERA) {

		if (!isCameraRecord) {
			// open a new file
			// QStandardPaths::MoviesLocation
			timer->stop();
			QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), ".", tr("Mp4 Files (*.mp4)"));
			if (!filename.toLower().contains(".mp4"))
				filename += ".mp4";
			// QMessageBox msgBox;
			// msgBox.setText(filename);
			// msgBox.exec();

			videoWriter.open(filename.toLocal8Bit().data(), VideoWriter::fourcc('D', 'I', 'V', '3'), 10, Size(fix_width, fix_height), true);
			m_button_record->setStyleSheet("border:3px solid black;background-color:red");
			timer->start();
			isCameraRecord = true;
		}
		else {
			// close file
			m_button_record->setStyleSheet("border:3px solid darkgray;background-color:white");
			videoWriter.release();
			isCameraRecord = false;
		}
	}
}

void MainWindow::multiButtonClicked()
{
	DisplayCh(0);
}

// not used now
void MainWindow::onMousePressed(QMouseEvent *event)
{
	qDebug("Pressed - x: %d, y: %d", event->x(), event->y());
	QString str;
	prevMousePos = event->pos();
}


void MainWindow::onMouseReleased(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		mouseState = 0;
	}
	else {
		QPoint item = pLabel->mapToGlobal(event->pos());
		QMenu submenu;
		submenu.addAction("Save");
		if (DEBUG_MODE) {
			submenu.addAction("Load");
			submenu.addAction("Export Standard Panorama");

		}
		QAction* rightClickItem = submenu.exec(item);
		if (rightClickItem && rightClickItem->text().contains("Save"))
		{
			QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "save.jpg", tr("JPEG Files (*.jpg)"));
			if (!filename.isEmpty()) {
				if (!filename.toLower().contains(".jpg"))
					filename += ".jpg";
				char *path = filename.toLocal8Bit().data();
				imwrite(path, image_result);
			}
		}
		else if (rightClickItem && rightClickItem->text().contains("Load"))
		{
			QString filename = QFileDialog::getOpenFileName(this, tr("Load file"), ".", tr("Image Files (*.jpg *.png)"));
			if (!filename.isEmpty()) {
				char *path = filename.toLocal8Bit().data();
				image_result = imread(path, IMREAD_COLOR);
				switch (medicalState) {
				case MedicalState::ANYPOINT:
					cv::resize(image_result, image_display[0], Size(main_width, main_height));
					renderArea->setPos(currAlpha, currBeta);
					break;
				case MedicalState::PANORAMA:
					cv::resize(image_result, image_display[0], Size(main_width, main_height));
					break;
				}
				cvtColor(image_display[0], image_display[0], COLOR_BGR2RGB);
				DisplayWindow(image_display[0], pLabel, x_base, y_base, main_width, main_height);
			}
		}
		else if (rightClickItem && rightClickItem->text().contains("Export Standard Panorama"))
		{

			// md.oneFisheyePanorama(image_input, image_p );

			Mat image_fliped;
			flip(image_input, image_fliped, 1);
			Mat image_p = Mat(2688, 5376, CV_8UC3, Scalar(0, 0, 0));
			md.twoFisheyePanorama(image_input, image_fliped, image_p);

			QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "save.jpg", tr("JPEG Files (*.jpg)"));
			if (!filename.isEmpty()) {
				if (!filename.toLower().contains(".jpg"))
					filename += ".jpg";
				char *path = filename.toLocal8Bit().data();
				imwrite(path, image_p);

			}
			image_fliped.release();
			image_p.release();

		}
	}
}

void MainWindow::onDoubleClicked(QMouseEvent *event)
{
	resetClicked();
}

void MainWindow::onRenderareaClicked(QMouseEvent *event)
{
cv:Point aPos;
QPointF ab;
	qDebug("Renderarea Clicked - x: %d, y: %d", event->x(), event->y());
	qDebug("md - w: %f, h: %f", md.getImageWidth(), md.getImageHeight());
	qDebug("renderArea - w: %d, h: %d", renderArea->width(), renderArea->height());
	switch (medicalState) {
	case MedicalState::ANYPOINT:
		if (event->button() == Qt::LeftButton) {
			renderArea->setPos(event->pos());
			QPoint rePos = QPoint(event->pos().x()*md.getImageWidth() / renderArea->width(),
				event->pos().y()*md.getImageHeight() / renderArea->height());
			cv::Point Pos;
			Pos.x = rePos.x();
			Pos.y = rePos.y();
			aPos = md.getAlphaBetaFromPos((int)anypointState, Pos);
			currAlpha = (int)aPos.x;
			currBeta = (int)aPos.y;
			doAnyPoint();
		}
		break;
	case MedicalState::PANORAMA:
		if (event->button() == Qt::LeftButton) {
			renderArea->setPos(event->pos());
			ab = renderArea->getAlphaBeta(0);
			currPanoiCxAlpha = (int)ab.x();
			currPanoiCxBeta = (int)ab.y();
			doPanorama();
		}
		else {
			if (DEBUG_MODE) {
				QPoint item = renderArea->mapToGlobal(event->pos());
				QMenu submenu;
				submenu.addAction("Save Rotated");
				QAction* rightClickItem = submenu.exec(item);
				if (rightClickItem && rightClickItem->text().contains("Save Rotated"))
				{
                    md.revPanorama(mapX_MediPano, mapY_MediPano, image_result, image_reverse, PANORAMA_ALPHA_MAX, currPanoiCxBeta);
					QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "Rotated.jpg", tr("JPEG Files (*.jpg)"));
					if (!filename.isEmpty()) {
						if (!filename.toLower().contains(".jpg"))
							filename += ".jpg";
						char *path = filename.toLocal8Bit().data();
						imwrite(path, image_reverse);
					}
				}
			}
		}

		break;

	}
	DisplayOne(false);
}

void MainWindow::onMouseMoved(QMouseEvent *event)
{
	if (medicalState != MedicalState::ANYPOINT) return;
	if (mouseState == 0)
	{
		prevMousePos = event->pos();
		mouseState = 1;
		return;
	}
	int x_inc;
	int y_inc;
	QPoint Offset = event->pos() - prevMousePos;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		if (currAlpha >= 0)
			x_inc = -Offset.x() / currZoom;
		else
			x_inc = Offset.x() / currZoom;
		break;
	case AnyPointState::MODE_2:
		x_inc = -Offset.x() / currZoom;
		break;

	}
	y_inc = -Offset.y() / currZoom;
	if (((currAlpha - y_inc) >= -90) && ((currAlpha - y_inc) <= 90))
		currAlpha = currAlpha - y_inc;
	else if ((currAlpha - y_inc) > 90)
		currAlpha = 90;
	else
		currAlpha = -90;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currBeta = (currBeta + x_inc) % 360;
		if (currBeta < 0)
			currBeta += 360;
		break;
	case AnyPointState::MODE_2:
		currBeta = currBeta + x_inc;
		if (currBeta > 90)
			currBeta = 90;
		else if (currBeta < -90)
			currBeta = -90;
		break;
	}

	doAnyPoint();
	DisplayOne(false);
	prevMousePos = event->pos();
}


void MainWindow::onListMouseMoved(QMouseEvent *event)
{
	QPoint pos = event->pos();
	QListWidgetItem *item = ui->listWidget->itemAt(pos);
	if (!item)
		return;
	// QToolTip::showText(ui->listWidget->viewport()->mapToGlobal(pos), "Sample tool tip using mouseMoveEvent in TableDialog class");

}

void MainWindow::onWheeled(QWheelEvent *event)
{
	if (medicalState != MedicalState::ANYPOINT) return;

	QString str;
	double w = (double)event->delta() / 200.0;
	if (w > 0) {
		if (currZoom + w < maxZoom)
			currZoom += w;
		else
			currZoom = maxZoom;
	}
	else if (w < 0) {
		if (currZoom + w >= minZoom)
			currZoom += w;
		else
			currZoom = minZoom;
	}
	doAnyPoint();
	DisplayOne(false);
	renderArea->setSize((int)(100 / sqrt(currZoom)));
}

void MainWindow::onRenderareaDoubleClicked(QMouseEvent *event)
{
	resetClicked();
}

void MainWindow::ch1ButtonClicked()
{
	DisplayCh(1);
}
void MainWindow::ch2ButtonClicked()
{
	DisplayCh(2);
}
void MainWindow::ch3ButtonClicked()
{
	DisplayCh(3);
}
void MainWindow::ch4ButtonClicked()
{
	DisplayCh(4);
}
void MainWindow::ch5ButtonClicked()
{
	DisplayCh(5);
}
void MainWindow::ch6ButtonClicked()
{
	DisplayCh(6);
}

void MainWindow::openCamera()
{
	// cap0 = cv::VideoCapture(0);
	// const std::string videoStreamAddress = m_pEdit->text().toUtf8().constData() ;
	const std::string videoStreamAddress = videoStreamURL.toUtf8().constData();

	if (FORCE_CAMERA_USB) {
		cap0.open(0);
		cap0.set(CAP_PROP_FRAME_WIDTH, 1920);
		cap0.set(CAP_PROP_FRAME_HEIGHT, 1080);
		if (cap0.isOpened()) {
			if (currPara != 1) {
				currPara = 1;
				loadParameterJson(Para[currPara]);
				fix_width = md.getImageWidth();
				fix_height = md.getImageHeight();
				initMat();
			}
		}
	}
	else {
		cap0.open(videoStreamAddress);
		if (cap0.isOpened()) {
			if (currPara != 0) {
				currPara = 0;
				loadParameterJson(Para[currPara]);
				fix_width = md.getImageWidth();
				fix_height = md.getImageHeight();
				initMat();
			}
		}
	}

	if (cap0.isOpened()) {
		timer->setInterval(200);
		timer->start();
		CaptureState = true;
		m_button_cam->setStyleSheet("border:5px solid darkgray;background-color: lightgray;");
		//if ( MOIL_APP == MoilApp::MEDICAL )
		//    medicalState = MedicalState::ANYPOINT ;
		mediaType = MediaType::CAMERA;
		currMetaPath = "camera_note";
		char *path0 = currMetaPath.toLocal8Bit().data();
		QDir qdir = QDir(currMetaPath);
		if (!qdir.exists())
			qdir.mkdir(currMetaPath);

		currMetaDirName = QDate::currentDate().toString("yyyy-MM-dd");
		currMetaFullPath = currMetaPath + "/" + currMetaDirName;
		// char *path = currMetaFullPath.toLocal8Bit().data();
		QDir qdir2 = QDir(currMetaFullPath);
		if (!qdir2.exists())
			qdir2.mkdir(currMetaFullPath);

		dbConnect(currMetaFullPath + "/moil.db");
		reSnapshotList();

	}

}



void MainWindow::closeCamera()
{
	CaptureState = false;
	m_button_cam->setStyleSheet("border:3px solid darkgray;background-color: white;");
	timer->stop();
	cap0.release();
	if (isCameraRecord) {
		isCameraRecord = false;
		videoWriter.release();
		m_button_record->setStyleSheet("border:3px solid darkgray;background-color:white");
	}
	ui->time1->setText("00:00");
	ui->time2->setText("00:00");
	mediaType = MediaType::NONE;
}

void MainWindow::closeVideo()
{
	CaptureState = false;
	timer->stop();
	cap0.release();
	ui->time1->setText("00:00");
	ui->time2->setText("00:00");
	ui->horizontalSlider->setValue(0);
	ui->horizontalSlider->setEnabled(false);
	mediaType = MediaType::NONE;
}

void MainWindow::readFrame()
{
	Mat image_temp;
	if (cap0.isOpened()) {
		switch (mediaType) {
		case MediaType::CAMERA:
			cap0.read(image_temp);
			if (image_temp.empty()) {
				closeCamera();
				return;
			}
			else {
				image_input.release();
				image_input = image_temp.clone();
				if (isCameraRecord)
					videoWriter.write(image_input);


			}
			break;
		case MediaType::VIDEO_FILE:
			cap0.read(image_temp);
			videoInfo.pos_frame = cap0.get(CAP_PROP_POS_FRAMES);
			videoInfo.pos_msec = cap0.get(CAP_PROP_POS_MSEC);
			if (image_temp.empty()) {
				playState = PlayState::STOP;
				pixmap = new QPixmap(":/images/play3.svg");
				ui->pushButton_play->setIcon(QIcon(*pixmap));
				videoInfo.frame_count = videoInfo.pos_frame + 1;
				ui->horizontalSlider->setValue(ui->horizontalSlider->maximum());
				timer->stop();
				return;
			}
			else {
				image_input.release();
				image_input = image_temp.clone();
				int dst_value = videoInfo.pos_frame * (ui->horizontalSlider->maximum() + 1) / videoInfo.frame_count;
				ui->horizontalSlider->blockSignals(true);
				ui->horizontalSlider->setValue(dst_value);
				ui->horizontalSlider->blockSignals(false);
				int sec = videoInfo.pos_frame / videoInfo.fps;
				int min = sec / 60; sec %= 60;
				QString fmt_time;
				fmt_time.sprintf("%02d:%02d", min, sec);
				ui->time1->setText(fmt_time);
			}
			break;
		}
		// !! it could waste computing power if the input resolution and preferred resolution do not match
		if ((image_input.cols != fix_width) || (image_input.rows != fix_height))
			cv::resize(image_input, image_input, Size(fix_width, fix_height));
		switch (MOIL_APP) {
		case MoilApp::CAR:
			DisplayCh(currCh);
			break;
		case MoilApp::MEDICAL:
			DisplayOne(true);
			break;
		}
	}
}


MainWindow::~MainWindow()
{

}

void MainWindow::openImageUi()
{

	if (cap0.isOpened()) {
		if (mediaType == MediaType::CAMERA)
			closeCamera();
		else if (mediaType == MediaType::VIDEO_FILE)
			closeVideo();
	}

	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
	if (!filename.isEmpty())
		openImage(filename);

}


void MainWindow::openImage(QString filename)
{

	if (cap0.isOpened()) {
		if (mediaType == MediaType::CAMERA)
			closeCamera();
		else if (mediaType == MediaType::VIDEO_FILE)
			closeVideo();
	}

	if (!filename.isEmpty()) {
		readImage(filename);
		lastOpendFile = filename;
		mediaType = MediaType::IMAGE_FILE;
		switch (MOIL_APP) {
		case MoilApp::CAR:
			DisplayCh(currCh);
			break;
		case MoilApp::MEDICAL:
			medicalState = MedicalState::ANYPOINT;
			refreshMedicalState();
			resetClicked();
			DisplayOne(true);
		}
		ui->listWidget->clear();
	}

}

void MainWindow::openVideoUi()
{
	//if ( MOIL_APP == MoilApp::MEDICAL ) {
	if (cap0.isOpened() && mediaType == MediaType::CAMERA) {
		closeCamera();
	}

	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open Image"), ".", tr("Video Files (*.mp4)"));
	if (!filename.isEmpty()) {
		QApplication::setOverrideCursor(Qt::WaitCursor);
		openVideo(filename);
		/*
		QFileInfo fi(filename);
		currMetaPath = fi.dir().path();
		currMetaDirName = fi.baseName() + "_note";
		currMetaFullPath = currMetaPath + "/" + currMetaDirName ;

		if (!DISCRETE_VIDEO_NOTE)
			currMetaFullPath = "output";
		else {
			char *path = currMetaFullPath.toLocal8Bit().data();
			DIR* dir = opendir(path);
			if (!dir)
			{
		#ifdef _WIN32
			mkdir(path);
		#else
			mkdir(path,0755);
		#endif
			}
			dbConnect(currMetaFullPath + "/moil.db");
			reSnapshotList();
		}
*/
		QApplication::restoreOverrideCursor();

	}
	//    }
}

void MainWindow::openVideo(QString filename)
{
	if (cap0.isOpened() && mediaType == MediaType::CAMERA) {
		closeCamera();
	}
	if (!filename.isEmpty()) {
		const std::string videoStreamAddress = filename.toUtf8().constData();
		if (cap0.isOpened())
			cap0.release();
		cap0.open(videoStreamAddress);
		if (cap0.isOpened()) {


			videoInfo.filename = lastOpendFile = filename;
			videoInfo.frame_width = cap0.get(CAP_PROP_FRAME_WIDTH);
			videoInfo.frame_height = cap0.get(CAP_PROP_FRAME_HEIGHT);
			videoInfo.frame_count = cap0.get(CAP_PROP_FRAME_COUNT);
			videoInfo.avi_ratio = cap0.get(CAP_PROP_POS_AVI_RATIO);
			videoInfo.fps = cap0.get(CAP_PROP_FPS);
			videoInfo.pos_frame = cap0.get(CAP_PROP_POS_FRAMES);
			videoInfo.pos_msec = cap0.get(CAP_PROP_POS_MSEC);
			mediaType = MediaType::VIDEO_FILE;
			if ((videoInfo.frame_width == 2592) && (videoInfo.frame_height == 1944) && currPara != 0) {
				currPara = 0;
				loadParameterJson(Para[currPara]);
				fix_width = md.getImageWidth();
				fix_height = md.getImageHeight();
				initMat();
			}
			else if ((videoInfo.frame_width == 1920) && (videoInfo.frame_height == 1080) && currPara != 1) {
				currPara = 1;
				loadParameterJson(Para[currPara]);
				fix_width = md.getImageWidth();
				fix_height = md.getImageHeight();
				initMat();
			}


			int sec = videoInfo.frame_count / videoInfo.fps;
			int min = sec / 60; sec %= 60;
			QString fmt_time;
			fmt_time.sprintf("%02d:%02d", min, sec);
			ui->time2->setText(fmt_time);

			CaptureState = true;
			// m_button_cam->setStyleSheet("border:5px solid darkgray;background-color: lightgray;");
			// videoSource = VideoSource::FIfilenameLE;
			if (MOIL_APP == MoilApp::MEDICAL)
				renderArea->setSize((int)(100 / sqrt(currZoom)));

			timer->setInterval(200);
			playState = PlayState::STOP;
			timer->stop();
			readFrame();
			pixmap = new QPixmap(":/images/play3.svg");
			ui->pushButton_play->setIcon(QIcon(*pixmap));
			ui->horizontalSlider->setEnabled(true);
		}


		switch (MOIL_APP) {
		case MoilApp::CAR:
			// DisplayCh(currCh);
			break;
		case MoilApp::MEDICAL:
			medicalState = MedicalState::ANYPOINT;
			refreshMedicalState();
			resetClicked();

			QFileInfo fi(filename);
			currMetaPath = fi.dir().path();
			currMetaDirName = fi.baseName() + "_note";
			currMetaFullPath = currMetaPath + "/" + currMetaDirName;

			if (!DISCRETE_VIDEO_NOTE)
				currMetaFullPath = "output";
			else {
				//char *path = currMetaFullPath.toLocal8Bit().data();
				QDir qdir = QDir(currMetaFullPath);
				if (!qdir.exists())
					qdir.mkdir(currMetaFullPath);

				dbConnect(currMetaFullPath + "/moil.db");
				reSnapshotList();
			}


		}


	}

}

void MainWindow::openParameterUi()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open Parameters"), ".", tr("Json Files (*.json)"));
	if (!filename.isEmpty()) {
		openParameter(filename);
	}
	switch (MOIL_APP) {
	case MoilApp::CAR:
		DisplayCh(0);
		break;
	case MoilApp::MEDICAL:
		renderArea->setCenter(QPointF(md.getiCx() / md.getImageWidth(), md.getiCy() / md.getImageHeight()), RadiusF[currPara]);
		renderArea->setAnypointPoly(anyPoly, anyPolyLength, anyPolyTopEdgeStart, anyPolyTopEdgeEnd, anyPolyCenter);
		DisplayOne(true);
	}
}

void MainWindow::openParameter(QString filename)
{

	if (!filename.isEmpty()) {

		if (cap0.isOpened()) {
			if (mediaType == MediaType::CAMERA)
				closeCamera();
			else if (mediaType == MediaType::VIDEO_FILE)
				closeVideo();
		}

		std::string fname = filename.toUtf8().constData();
		ifstream inFile;
		inFile.open(fname);

		stringstream strStream;
		strStream << inFile.rdbuf(); // Read the file
		string string = strStream.str();
		loadParameterJson(string);
		QMessageBox msgBox;
		QString AA = QString::fromStdString(string);
		msgBox.setText(AA);
		msgBox.exec();
	}
}

void MainWindow::openNoteUi()
{
	if (cap0.isOpened()) {
		if (mediaType == MediaType::CAMERA)
			closeCamera();
		else if (mediaType == MediaType::VIDEO_FILE)
			closeVideo();
	}

	QString filename = QFileDialog::getExistingDirectory(0, ("Select Folder"), QDir::currentPath());
	if (!filename.isEmpty())
		openNote(filename);
}

void MainWindow::openNote(QString filename)
{

	if (!QFileInfo::exists(filename + "/moil.db"))
	{
		QMessageBox msgBox;
		msgBox.setText("Invalid");
		msgBox.exec();
		return;
	}

	currMetaFullPath = filename;
	dbConnect(currMetaFullPath + "/moil.db");
	reSnapshotList();

}

void MainWindow::loadParameterJson(string str)
{
    QString str1 = QString::fromStdString(str);
    QByteArray jsonData = str1.toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject Para = jsonDoc.object();

    // json Para = json::parse(str);
    md.Config(Para["cameraName"].toString().toStdString(), Para["cameraSensorWidth"].toDouble(), Para["cameraSensorHeight"].toDouble(),
        Para["iCx"].toDouble(), Para["iCy"].toDouble(), Para["ratio"].toDouble(),
        Para["imageWidth"].toDouble(), Para["imageHeight"].toDouble(), Para["calibrationRatio"].toDouble(),
        Para["parameter0"].toDouble(),
        Para["parameter1"].toDouble(),
        Para["parameter2"].toDouble(),
        Para["parameter3"].toDouble(),
        Para["parameter4"].toDouble(),
        Para["parameter5"].toDouble()
	);


	// debug
	// ConfigData* c = md.getcd();
	// double dd = c->getAlphaFromRho(611);
	// qDebug() << "Debug : " << c->rhoToAlpha_Table[611] ;


}

void MainWindow::on_actionLoad_Image_triggered()
{
	openImageUi();
}

void MainWindow::on_actionLoad_Video_triggered()
{
	openVideoUi();
}

void MainWindow::on_actionExit_triggered()
{
	cap0.release();
	saveSettings();
	QCoreApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	saveSettings();
}

void MainWindow::on_actionMOIL_triggered()
{
	// QMessageBox::information(NULL,"About","MOIL \n\n\n Ming Chi University of Technology",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);

	QMessageBox msgBox;
	msgBox.setText("MOIL \n\n\nOmnidirectional Imaging & Survaillance Lab\n\nMing Chi University of Technology\n\n");
	msgBox.setIconPixmap(QPixmap(":/images/chess.jpg"));
	msgBox.exec();

}

void MainWindow::on_actionLoad_triggered()
{
	openParameterUi();
}


void MainWindow::loadSettings()
{
	QSettings settings(m_sSettingsFile, QSettings::IniFormat);
	videoStreamURL = settings.value("videoSourceURL", "").toString();
	cacheLocation = settings.value("cacheLocation", "").toString();
	// QString sVideoSourceFile = settings.value("videoSourceFile", "").toString();
	QString lastOpenedFile_ = settings.value("lastOpenedFile", "").toString();
	if (videoStreamURL == "")
		videoStreamURL = QString::fromStdString(default_videoStreamURL);
	if (cacheLocation == "")
		cacheLocation = QString::fromStdString(default_cacheLocation);
	/*
	 if ( sVideoSourceFile != "" ) {
			openVideo(sVideoSourceFile);
	 }
	*/
	if (lastOpenedFile_ != "") {
		if (lastOpenedFile_.toLower().contains(".jpg")) {
			mediaType = MediaType::IMAGE_FILE;
			openImage(lastOpenedFile_);
		}
		else if (lastOpenedFile_.toLower().contains(".mp4")) {
			mediaType = MediaType::VIDEO_FILE;
			openVideo(lastOpenedFile_);
		}
	}

	if (m_pEdit)
		m_pEdit->setText(videoStreamURL);
}

void MainWindow::saveSettings()
{
	QSettings settings(m_sSettingsFile, QSettings::IniFormat);
	// QString sText = (m_pEdit) ? m_pEdit->text() : "";
	// settings.setValue("videoSourceURL", sText);
	settings.setValue("videoSourceURL", videoStreamURL);
	settings.setValue("cacheLocation", cacheLocation);
	/*
	if( cap0.isOpened() && ( videoSource == VideoSource::FILE)) {
	   settings.setValue("videoSourceFile", videoInfo.filename);
	   cap0.release();
	}
	else
	   settings.setValue("videoSourceFile", "");
   */

	if ((mediaType == MediaType::IMAGE_FILE) || (mediaType == MediaType::VIDEO_FILE))
		settings.setValue("lastOpenedFile", lastOpendFile);
	else
		settings.setValue("lastOpenedFile", "");
}

void MainWindow::on_actionVideo_Source_triggered()
{

}

void MainWindow::onListItemClicked(QListWidgetItem* item)
{

	// show snapshot 
	QString filename(item->text());

	/*
	QString str = "border-image:url(snapshot/" + filename + ")0 0 0 0 stretch stretch;";
	QDialog *subDialog = new QDialog;
	subDialog->setWindowTitle("Snapshot");
	subDialog->resize(screen.width()*0.8, screen.height()*0.8);
	subDialog->setStyleSheet( str );
	subDialog->show();
	*/
	int a, b, pos_frame, vt, p;
	float z;
	QString vidsrc;
	QString comment;

	bool Ret = dbGetRecord(filename, &a, &b, &z, &pos_frame, &vt, &vidsrc, &p, &comment, true);
	if (p != currPara) {
		currPara = p;
		loadParameterJson(Para[currPara]);
		fix_width = md.getImageWidth();
		fix_height = md.getImageHeight();
		initMat();
	}

	if (Ret) {
		if (cap0.isOpened()) {
			if (mediaType == MediaType::CAMERA)
				closeCamera();
			else {
				playState = PlayState::STOP;
				timer->stop();
			}
		}



		switch ((MediaType)vt) {
		case MediaType::CAMERA: // single frame

			readImage(currMetaFullPath + "/" + filename);

			switch (MOIL_APP) {
			case MoilApp::CAR:
				DisplayCh(currCh);
				break;
			case MoilApp::MEDICAL:
				medicalState = MedicalState::ANYPOINT;
				refreshMedicalState();
				currAlpha = a;
				currBeta = b;
				currZoom = z;
				doAnyPoint();
				DisplayOne(true);
			}

			break;
		case MediaType::VIDEO_FILE:

			switch (MOIL_APP) {
			case MoilApp::CAR:
				DisplayCh(currCh);
				break;
			case MoilApp::MEDICAL:
				// medicalState = MedicalState::ANYPOINT ;
				refreshMedicalState();
				currAlpha = a;
				currBeta = b;
				currZoom = z;
				doAnyPoint();
				if (DISCRETE_VIDEO_NOTE) {
					vidsrc = vidsrc.mid(vidsrc.lastIndexOf("/"));
					vidsrc = currMetaPath + "/" + vidsrc;
				}

				if (cap0.isOpened() && vidsrc == videoInfo.filename) {
					cap0.set(CAP_PROP_POS_FRAMES, pos_frame);
					readFrame();
					DisplayOne(true);
				}
				else {
					// try to open video file
					FILE* pf = fopen(vidsrc.toLocal8Bit().data(), "r");
					if (pf)
					{
						fclose(pf);
						openVideo(vidsrc);
						if (cap0.isOpened()) {
							cap0.set(CAP_PROP_POS_FRAMES, pos_frame);
							readFrame();
						}
					}
					// if fail to open video file, use still image            
					readImage(currMetaFullPath + "/" + filename);
					DisplayOne(true);
				}

			}

			break;
		}



	}
}

void MainWindow::upClicked()
{
	if (medicalState == MedicalState::ANYPOINT)
	{

		if (currAlpha + currInc <= 90)
			currAlpha = currAlpha + currInc;
		else
			currAlpha = 90;
		doAnyPoint();
		DisplayOne(false);

	}
}

void MainWindow::downClicked()
{
	if (medicalState == MedicalState::ANYPOINT)
	{
		if (currAlpha - currInc >= -90)
			currAlpha = currAlpha - currInc;
		else
			currAlpha = -90;
		doAnyPoint();
		DisplayOne(false);
	}
}

void MainWindow::leftClicked()
{
	if (medicalState == MedicalState::ANYPOINT)
	{
		switch (anypointState) {
		case AnyPointState::MODE_1:
			if (currBeta - currInc >= 0)
				currBeta = currBeta - currInc;
			else
				currBeta = (currBeta - currInc) + 360;
			break;
		case AnyPointState::MODE_2:
			currBeta = currBeta - currInc;
			if (currBeta > 90)
				currBeta = 90;
			else if (currBeta < -90)
				currBeta = -90;
			break;
		}
		doAnyPoint();
		DisplayOne(false);
	}
}

void MainWindow::rightClicked()
{
	if (medicalState == MedicalState::ANYPOINT)
	{
		switch (anypointState) {
		case AnyPointState::MODE_1:
			currBeta = (currBeta + currInc) % 360;
			if (currBeta < 0) currBeta += 360;
			break;
		case AnyPointState::MODE_2:
			currBeta = currBeta + currInc;
			if (currBeta > 90)
				currBeta = 90;
			else if (currBeta < -90)
				currBeta = -90;
			break;
		}
		doAnyPoint();
		DisplayOne(false);
	}
}

void MainWindow::refreshMedicalState()
{
	switch (medicalState) {
	case MedicalState::ORIGINAL:
		// m_original->setStyleSheet("border:5px solid red;");
		m_anypoint->setStyleSheet("border:3px solid darkgray;");
		m_panorama->setStyleSheet("border:3px solid darkgray;");
		break;
	case MedicalState::ANYPOINT:
		// m_original->setStyleSheet("border:5px solid darkgray;");
		m_anypoint->setStyleSheet("border:3px solid red;");
		m_panorama->setStyleSheet("border:3px solid darkgray;");
		break;
	case MedicalState::PANORAMA:
		// m_original->setStyleSheet("border:5px solid darkgray;");
		m_anypoint->setStyleSheet("border:3px solid darkgray;");
		m_panorama->setStyleSheet("border:3px solid red;");
		break;
	}
}

void MainWindow::calcAnypointPoly()
{
	int w = mapX_Medi.cols;
	int h = mapX_Medi.rows;
	int w_step = w / 10;
	int h_step = h / 10;
	QPointF anyPoly0[anyPoly_Max];

	anyPoly0[0].setX((*(float *)mapX_Medi.data) / w);
	anyPoly0[0].setY((*(float *)mapY_Medi.data) / h);
	for (int i = 1; i < 10; i++) {
		anyPoly0[i].setX((*((float *)mapX_Medi.data + w_step * i)) / w);
		anyPoly0[i].setY((*((float *)mapY_Medi.data + w_step * i)) / h);
	}
	anyPoly0[10].setX((*((float *)mapX_Medi.data + w - 1)) / w);
	anyPoly0[10].setY((*((float *)mapY_Medi.data + w - 1)) / h);
	for (int i = 11; i < 20; i++) {
		anyPoly0[i].setX((*((float *)mapX_Medi.data + w * h_step*(i - 10) + w - 1)) / w);
		anyPoly0[i].setY((*((float *)mapY_Medi.data + w * h_step*(i - 10) + w - 1)) / h);
	}
	anyPoly0[20].setX((*((float *)mapX_Medi.data + w * h - 1)) / w);
	anyPoly0[20].setY((*((float *)mapY_Medi.data + w * h - 1)) / h);
	for (int i = 21; i < 30; i++) {
		anyPoly0[i].setX((*((float *)mapX_Medi.data + w * h - 1 - (i - 20)*w_step)) / w);
		anyPoly0[i].setY((*((float *)mapY_Medi.data + w * h - 1 - (i - 20)*w_step)) / h);
	}
	anyPoly0[30].setX((*((float *)mapX_Medi.data + w * (h - 1))) / w);
	anyPoly0[30].setY((*((float *)mapY_Medi.data + w * (h - 1))) / h);
	for (int i = 31; i < 40; i++) {
		anyPoly0[i].setX((*((float *)mapX_Medi.data + w * h_step*(40 - i))) / w);
		anyPoly0[i].setY((*((float *)mapY_Medi.data + w * h_step*(40 - i))) / h);
	}
	anyPolyLength = 40;
	anyPolyTopEdgeStart = 0;
	anyPolyTopEdgeEnd = 10;
	// remove (0,0) points
	int cnt = 0;
	for (int i = 0; i < anyPolyLength; i++) {
		if ((anyPoly0[i].x() > 0) || (anyPoly0[i].y() > 0)) {
			anyPoly[cnt].setX(anyPoly0[i].x());
			anyPoly[cnt].setY(anyPoly0[i].y());
			cnt++;
		}
		else {
			if ((i == anyPolyTopEdgeStart) && (anyPolyTopEdgeStart < anyPolyTopEdgeEnd))
				anyPolyTopEdgeStart += 1;
		}
	}

	anyPolyLength = cnt;
	if (anyPolyTopEdgeStart == anyPolyTopEdgeEnd) {
		anyPolyTopEdgeStart = 0;
		anyPolyTopEdgeEnd = 0;
	}
	if (DEBUG_MODE) {
		anyPolyCenter.setX((*((float *)mapX_Medi.data + (w*h / 2 - (w / 2)))) / w);
		anyPolyCenter.setY((*((float *)mapY_Medi.data + (w*h / 2 - (w / 2)))) / h);
	}
	else {
		anyPolyCenter = QPointF(0, 0);
	}

}

void MainWindow::resetClicked()
{
	switch (medicalState) {
	case MedicalState::ANYPOINT:
		currAlpha = 0;
		currBeta = 0;
		currZoom = defaultZoom;
		doAnyPoint();
		break;
	case MedicalState::PANORAMA:
		currPanoiCxAlpha = 0;
		currPanoiCxBeta = 0;
		renderArea->setPos(currPanoiCxAlpha, currPanoiCxBeta);
		renderArea->setPanorama();
		doPanorama();
	}
	DisplayOne(false);
}

// Deprecated
void MainWindow::originalClicked(QMouseEvent*)
{
	return;
	medicalState = MedicalState::ORIGINAL;
	refreshMedicalState();
	DisplayOne(false);
}

void MainWindow::anypointClicked()
{
	medicalState = MedicalState::ANYPOINT;
	refreshMedicalState();
	DisplayOne(false);
}

void MainWindow::radioButtonMode1Clicked()
{
	medicalState = MedicalState::ANYPOINT;
	if (anypointState == AnyPointState::MODE_2) {
		if ((currAlpha < -90) || (currAlpha > 90) || (currBeta < 0) || (currBeta > 360)) {
			currAlpha = 0;
			currBeta = 0;
		}
		else {
			currAlpha = (currAlpha < -90) ? -90 : currAlpha;
			currAlpha = (currAlpha > 90) ? 90 : currAlpha;
			currBeta = (currBeta < 0) ? 0 : currBeta;
			currBeta = (currBeta > 360) ? 360 : currBeta;
		}
		anypointState = AnyPointState::MODE_1;
		doAnyPoint();
	}
	refreshMedicalState();
	DisplayOne(false);
}

void MainWindow::radioButtonMode2Clicked()
{
	medicalState = MedicalState::ANYPOINT;
	if (anypointState == AnyPointState::MODE_1) {
		if ((currAlpha < -90) || (currAlpha > 90) || (currBeta < -90) || (currBeta > 90)) {
			currAlpha = 0;
			currBeta = 0;
		}
		else {
			currAlpha = (currAlpha < -90) ? -90 : currAlpha;
			currAlpha = (currAlpha > 90) ? 90 : currAlpha;
			currBeta = (currBeta < -90) ? -90 : currBeta;
			currBeta = (currBeta > 90) ? 90 : currBeta;
		}
		anypointState = AnyPointState::MODE_2;
		doAnyPoint();
	}
	refreshMedicalState();
	DisplayOne(false);
}

void MainWindow::panoramaClicked()
{
	medicalState = MedicalState::PANORAMA;
	refreshMedicalState();
	renderArea->setPos(currPanoiCxAlpha, currPanoiCxBeta);
	renderArea->setPanorama();
	DisplayOne(false);
}


bool MainWindow::dbConnect(const QString &dbName)
{
	if (db.open())
		db.close();

	db.setDatabaseName(dbName);
	if (!db.open()) {
		qDebug() << "Database Error!";
		return false;
	}
	QSqlQuery query;
	if (!query.exec("CREATE TABLE if not exists imageinfo ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"filename VARCHAR,"
		"alpha INT,"
		"beta INT,"
		"zoom FLOAT,"
		"pos_frame INT,"
		"vidtype INT,"
		"vidsrc VARCHAR,"
		"para_index INT,"
		"medicalstate INT,"
		"comment VARCHAR,"
		"parameter VARCHAR)")) {
		return false;
	}

	return true;
}

bool MainWindow::dbAddRecord(QString filename, int alpha, int beta, float zoom, int pos_frame, int vidType, QString vidSrc, int para_idx, QString comment)
{
	QSqlQuery query;
	QString str;
	str.sprintf("INSERT INTO imageinfo (filename, alpha, beta, zoom, pos_frame, vidtype, vidsrc, para_index, medicalstate, comment) VALUES (\"%s\", %d, %d, %f, %d, %d,\"%s\", %d, %d,\"%s\")", filename.toUtf8().constData(), alpha, beta, zoom, pos_frame, vidType, vidSrc.toUtf8().constData(), para_idx, (int)medicalState, comment.toUtf8().constData());
	if (!query.exec(str)) {
		return false;
	}
	return true;
}

bool MainWindow::dbGetRecord(QString filename, int *alpha, int *beta, float *zoom, int *pos_frame, int *vidType, QString *vidSrc, int *para_idx, QString *comment, bool changeState)
{
	QSqlQuery query;
	QString str;
	str.sprintf("SELECT * FROM imageinfo WHERE filename =\"%s\"", filename.toUtf8().constData());
	if (!query.exec(str)) {
		qDebug() << "Failed!";
		return false;
	}
	else {
		while (query.next())
		{
			qDebug() << query.value(0).toInt() << query.value(1).toString() << query.value(2).toInt() << query.value(3).toInt() << query.value(4).toFloat();
			*alpha = query.value(2).toInt();
			*beta = query.value(3).toInt();
			*zoom = query.value(4).toFloat();
			*pos_frame = query.value(5).toInt();
			*vidType = query.value(6).toInt();
			*vidSrc = query.value(7).toString();
			*para_idx = query.value(8).toInt();
			if (changeState)
				medicalState = (MedicalState)query.value(9).toInt();
			*comment = query.value(10).toString();
		}
	}
	return true;
}

bool MainWindow::dbDelRecord(QString filename)
{
	QSqlQuery query;
	QString str;
	str.sprintf("DELETE FROM imageinfo WHERE filename =\"%s\"", filename.toUtf8().constData());
	if (!query.exec(str)) {
		return false;
	}
	return true;
}

bool MainWindow::dbSetComment(QString filename, QString comment)
{
	QSqlQuery query;
	QString str;
	str.sprintf("UPDATE imageinfo SET comment=\"%s\" WHERE filename =\"%s\"", comment.toUtf8().constData(), filename.toUtf8().constData());
	if (!query.exec(str)) {
		return false;
	}
	return true;
}

void MainWindow::on_horizontalSlider_actionTriggered(int action)
{

}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
	if (cap0.isOpened() && mediaType == MediaType::VIDEO_FILE) {
		int dst_frame = videoInfo.frame_count * value / ui->horizontalSlider->maximum();
		cap0.set(CAP_PROP_POS_FRAMES, dst_frame);
		if (playState == PlayState::STOP) {
			readFrame();
		}
		qDebug() << value << " : " << dst_frame;
	}
}

void MainWindow::loadStyleSheet(const QString &sheetName)
{
	QFile file(":/qss/" + sheetName.toLower() + ".qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QString::fromLatin1(file.readAll());
	qApp->setStyleSheet(styleSheet);

}

void MainWindow::on_pushButton_play_clicked()
{
	if (cap0.isOpened() && mediaType == MediaType::VIDEO_FILE) {
		switch (playState) {
		case PlayState::STOP:
			if (videoInfo.pos_frame >= videoInfo.frame_count - 1) {
				cap0.set(CAP_PROP_POS_FRAMES, 0);
			}
			timer->start();
			playState = PlayState::PLAY;
			pixmap = new QPixmap(":/images/pause2.svg");
			ui->pushButton_play->setIcon(QIcon(*pixmap));
			break;
		case PlayState::PLAY:
			timer->stop();
			playState = PlayState::STOP;
			pixmap = new QPixmap(":/images/play3.svg");
			ui->pushButton_play->setIcon(QIcon(*pixmap));
			break;
		}
	}
}

void MainWindow::on_pushButton_left_clicked()
{
	if (cap0.isOpened() && mediaType == MediaType::VIDEO_FILE) {
		if (playState == PlayState::PLAY) {
			timer->stop();
			playState = PlayState::STOP;
		}
		if (videoInfo.pos_frame > 1) {
			cap0.set(CAP_PROP_POS_FRAMES, videoInfo.pos_frame - 2);
			readFrame();
		}
	}
}

void MainWindow::on_pushButton_right_clicked()
{
	if (cap0.isOpened() && mediaType == MediaType::VIDEO_FILE) {
		if (playState == PlayState::PLAY) {
			timer->stop();
			playState = PlayState::STOP;
		}
		if (videoInfo.pos_frame < videoInfo.frame_count - 1) {
			int lastframe = videoInfo.pos_frame;
			readFrame();
			if (videoInfo.pos_frame == lastframe) {
				// videoInfo.frame_count = videoInfo.pos_frame + 1 ; // opencv bug
			}
		}
	}
}


void MainWindow::ListMenuRequested(const QPoint & pos)
{
	if (ui->listWidget->count() == 0) return;
	/*
		QPoint globalPos = ui->listWidget->mapToGlobal(pos);	// Map the global position to the userlist
		QModelIndex t = ui->listWidget->indexAt(pos);
		ui->listWidget->item(t.row())->setSelected(true);			// even a right click will select the item
		sMenu->exec(globalPos);
	*/
	QPoint item = ui->listWidget->mapToGlobal(pos);
	QModelIndex t = ui->listWidget->indexAt(pos);
	QListWidgetItem *this_item = ui->listWidget->item(t.row());
	QMenu submenu;
	submenu.addAction("Comment");
	submenu.addAction("Delete");
	QAction* rightClickItem = submenu.exec(item);

	if (rightClickItem && rightClickItem->text().contains("Comment"))
	{
		QDialog *dialog = new QDialog(this);
		QLabel *label = new QLabel;
		label->setText("Comment: ");
		QTextEdit *textedit = new QTextEdit(this);
		textedit->setText("");
		int a, b, pos_frame, vt, p;
		float z;
		QString vidsrc;
		QString comment;
		bool Ret = dbGetRecord(this_item->text(), &a, &b, &z, &pos_frame, &vt, &vidsrc, &p, &comment, false);
		if (Ret)
			textedit->setText(comment);
		QPushButton *ok = new QPushButton("Ok", dialog);
		QPushButton *cancel = new QPushButton("Cancel", dialog);
		QObject::connect(ok, SIGNAL(clicked()), dialog, SLOT(accept()));
		QObject::connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
		QVBoxLayout *VLayout = new QVBoxLayout(this);
		QHBoxLayout *HLayout1 = new QHBoxLayout(this);
		QHBoxLayout *HLayout2 = new QHBoxLayout(this);
		HLayout1->addWidget(label);
		HLayout1->addWidget(textedit);
		HLayout2->addWidget(ok);
		HLayout2->addWidget(cancel);
		dialog->setWindowTitle(tr("Input Comment"));
		VLayout->addLayout(HLayout1);
		VLayout->addLayout(HLayout2);
		dialog->setLayout(VLayout);
		dialog->exec();
		if (dialog->result() == QDialog::Accepted) {

			// QMessageBox msgBox;
			// msgBox.setText( textedit->toPlainText());
			// msgBox.exec();
			dbSetComment(this_item->text(), textedit->toPlainText());
			reSnapshotList();
		}

	}
	else if (rightClickItem && rightClickItem->text().contains("Delete"))
	{
		delRecord(this_item->text());
		ui->listWidget->takeItem(ui->listWidget->indexAt(pos).row());
	}
}


void MainWindow::httpFinished(QNetworkReply* reply) {

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        qDebug() << reply->readAll();
    }
}

void MainWindow::delRecord(QString filename) {
	char file[128];
	strcpy(&file[0], currMetaFullPath.toLocal8Bit().data());
	strcat(&file[0], "/");
	strcat(&file[0], filename.toLocal8Bit().data());
	remove(file);
	strcpy(&file[0], currMetaFullPath.toLocal8Bit().data());
	strcat(&file[0], "/thumbnail/");
	strcat(&file[0], filename.toLocal8Bit().data());
	remove(file);

	dbDelRecord(filename);
}

void MainWindow::on_pushButton_top_clicked()
{
	if (cap0.isOpened() && mediaType == MediaType::VIDEO_FILE) {
		if (playState == PlayState::PLAY) {
			timer->stop();
			playState = PlayState::STOP;
		}
		if (videoInfo.pos_frame > 1) {
			cap0.set(CAP_PROP_POS_FRAMES, 0);
			readFrame();
		}
	}
}

void MainWindow::on_ap_right_clicked()
{
	rightClicked();
}

void MainWindow::on_ap_up_clicked()
{
	upClicked();
}

void MainWindow::on_ap_left_clicked()
{
	leftClicked();
}

void MainWindow::on_ap_reset_clicked()
{
	resetClicked();
}

void MainWindow::on_ap_down_clicked()
{
	downClicked();
}

void MainWindow::on_actionCamera_URL_triggered()
{
	QDialog *dialog = new QDialog(this);
	QLabel *label = new QLabel;
	label->setText("Camera URL: ");
	QLineEdit *lineedit = new QLineEdit(this);
	lineedit->setText(videoStreamURL);
	QPushButton *ok = new QPushButton("Ok", dialog);
	QPushButton *cancel = new QPushButton("Cancel", dialog);
	QObject::connect(ok, SIGNAL(clicked()), dialog, SLOT(accept()));
	QObject::connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
	QVBoxLayout *VLayout = new QVBoxLayout(this);
	QHBoxLayout *HLayout1 = new QHBoxLayout(this);
	QHBoxLayout *HLayout2 = new QHBoxLayout(this);
	HLayout1->addWidget(label);
	HLayout1->addWidget(lineedit);

	HLayout2->addStretch();
	HLayout2->addWidget(ok, 0, Qt::AlignRight);
	HLayout2->addWidget(cancel, 0, Qt::AlignRight);
	dialog->setWindowTitle(tr("Camera URL"));
	VLayout->addLayout(HLayout1, 5);
	VLayout->addLayout(HLayout2, 1);
	dialog->setMinimumWidth(500);
	dialog->setLayout(VLayout);
	dialog->exec();
	if (dialog->result() == QDialog::Accepted) {
		if (dialog->result() == QDialog::Accepted) {
			videoStreamURL = lineedit->text();
		}

	}
}

void MainWindow::on_actionLoad_Note_triggered()
{
	openNoteUi();
}

void MainWindow::resizeEvent(QResizeEvent * e)
{
	QMainWindow::resizeEvent(e);
	if (MOIL_APP == MoilApp::MEDICAL) {
		renderArea->resize(m_original->size());
		DisplayOne(true);
	}

}

void MainWindow::doAnyPoint()
{
	if (anypointCacheInfo.cacheState == CacheState::DISABLED) {
		if (MOIL_APP == MoilApp::MEDICAL) {
			switch (anypointState) {
			case AnyPointState::MODE_1:
				md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
				break;
			case AnyPointState::MODE_2:
				md.AnyPointM2((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
				break;
			}
		}
	}
	else
		fetchAnypointCache(anypointState, currAlpha, currBeta);
}
// ref.  https://www.itread01.com/content/1544956741.html
// ref.  https://www.qt.io/blog/asynchronous-apis-in-qt-6
/*
void MainWindow::httpAnypoint(const QUrl &url) {

auto future = QtConcurrent::run(httpGetMatrix, url)
            .then(processMatrix)
            .onFailed([](QNetworkReply::NetworkError) {
                // handle network errors
            });

}

QByteArray MainWindow::httpGetMatrix(const QUrl &url)
{


  //  QNetworkRequest request;
  //  QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
  //  QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished(QNetworkReply*)));
  //  Q_ASSERT(connRet);

  //  request.setUrl(QUrl("http://localhost:1111"));
  //  QNetworkReply* reply = naManager->get(request);



        QNetworkReply *reply = manager.get(QNetworkRequest(url));
        QObject::connect(reply, &QNetworkReply::finished, [reply] {...});

        // wait until we've received all data
        // ...
        return data;

}

void MainWindow::requestFinished(QNetworkReply* reply) {

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        qDebug() << reply->readAll();
    }
}
*/
void MainWindow::doRemap(Mat &Src, Mat &Dest, Mat &mapX, Mat &mapY)
{

	// test AnyPoint + Remap, without mapX, MapY output
	/*
		if (( anypointCacheInfo.cacheState == CacheState::DISABLED ) && ( MOIL_APP == MoilApp::MEDICAL ) && ( anypointState == AnyPointState::MODE_2 ))
			md.AnyPoint2_Remap(Src, Dest, (double)currAlpha, (double)currBeta, currZoom, m_ratio );
		else
	*/
	md.Remap(Src, Dest, mapX, mapY);


	// local : remap( Src, Dest, mapX, mapY, INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
}

void MainWindow::doPanorama()
{
    md.PanoramaM_Rt((float *)mapX_MediPano.data, (float *)mapY_MediPano.data, mapX_MediPano.cols, mapX_MediPano.rows, m_ratio, PANORAMA_ALPHA_MAX, currPanoiCxAlpha, currPanoiCxBeta);
}

void MainWindow::genAnypointCache(QString dirname, AnyPointState mode, int zoom, int alpha_max, int alpha_step, int beta_step)
{
	// Mode 1 : alpha 0..90, beta 0..360
	// Mode 2 : alpha -90..90, beta -90..90
	// 1920x1080 11*10 = 32 Sec. 0.25 FPS
	// 2592x1944 11*10 = 68 Sec. 0.62 FPS
	//  90x360(files)x0.25s=2.25hr, x16.6M > 538G
	// 2200x220(files)x0.62s=8.33hr, x40.4M > 1.95T
	int alpha, alpha_start, alpha_end, beta, beta_start, beta_end;
	switch (mode) {
	case AnyPointState::MODE_1:
		alpha_start = 0;
		alpha_end = alpha_max;
		beta_start = 0;
		beta_end = 360;
		break;
	case AnyPointState::MODE_2:
		alpha_start = -alpha_max;
		alpha_end = alpha_max;
		beta_start = -alpha_max;
		beta_end = alpha_max;
		break;
	}

	QProgressDialog *pd;

	pd = new QProgressDialog("Generate Cache Files", "Cancel", 0, (alpha_end - alpha_start)*(beta_end - beta_start) / alpha_step / beta_step - 1);
	pd->setWindowModality(Qt::WindowModal);
	pd->setMinimumDuration(0);
	pd->setLabelText("Generating...");

	char dir_str[64], str[64], str2[64];
	strcpy(dir_str, dirname.toLocal8Bit().data());
	strcat(dir_str, "/cache");

	QDir qdir = QDir(dirname + "/cache");
	if (!qdir.exists())
		qdir.mkdir(dirname + "/cache");

	alpha = alpha_start;
	while (alpha < alpha_end && !pd->wasCanceled())
	{
		sprintf(str, "%s/A%d", dir_str, alpha);
		QDir qdir = QDir(str);
		if (!qdir.exists())
			qdir.mkdir(str);

		beta = beta_start;
		while (beta < beta_end && !pd->wasCanceled())
		{
			QString q_str = "Generating...(" + QString::number(alpha) + ", " + QString::number(beta) + ")";
			pd->setLabelText(q_str);
			sprintf(str, "%s/A%d/B%d", dir_str, alpha, beta);
			QDir qdir = QDir(str);
			if (!qdir.exists())
				qdir.mkdir(str);

			switch (mode) {
			case AnyPointState::MODE_1:
				md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)alpha, (double)beta, zoom, m_ratio);
				break;
			case AnyPointState::MODE_2:
				md.AnyPointM2((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)alpha, (double)beta, zoom, m_ratio);
				break;
			}
			sprintf(str2, "%s/matX", str);
			MatWrite(str2, mapX_Medi);
			sprintf(str2, "%s/matY", str);
			MatWrite(str2, mapY_Medi);
			pd->setValue((alpha - alpha_start / alpha_step) * (beta_end - beta_start) / beta_step + (beta - beta_start) / beta_step);
			beta += beta_step;
		}
		alpha += alpha_step;
	}
}

void MainWindow::on_actionGenerate_Cache_triggered()
{
	QString dirname = QFileDialog::getExistingDirectory(0, ("Select Folder"), QDir::currentPath());
	if (!dirname.isEmpty())
		genAnypointCache(dirname, AnyPointState::MODE_2, 6, 110, 1, 1);
}

void MainWindow::fetchAnypointCache(AnyPointState mode, int alpha, int beta)
{
	double zoom = 6;
	char dir_str[64], str[64], str_x[64], str_y[64];

	if (anypointCacheInfo.cacheState == CacheState::ENABLED) {

		QString fileMapX = cacheLocation + "/cache/A" + QString::number(alpha) + "/B" + QString::number(beta) + "/matX";
		QString fileMapY = cacheLocation + "/cache/A" + QString::number(alpha) + "/B" + QString::number(beta) + "/matY";

		if (QFileInfo::exists(fileMapX) && QFileInfo::exists(fileMapY)) {
			strcpy(dir_str, cacheLocation.toLocal8Bit().data());
			strcat(dir_str, "/cache");
			sprintf(str, "%s/A%d/B%d", dir_str, alpha, beta);
			sprintf(str_x, "%s/matX", str);
			sprintf(str_y, "%s/matY", str);
			mapX_Medi = MatRead(str_x);
			mapY_Medi = MatRead(str_y);
			return;
		}
	}
	switch (mode) {
	case AnyPointState::MODE_1:
		md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)alpha, (double)beta, zoom, m_ratio);
		break;
	case AnyPointState::MODE_2:
		md.AnyPointM2((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)alpha, (double)beta, zoom, m_ratio);
		break;
	}

}





void MainWindow::on_ap_DBG_F_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	currAlpha = 0;
	currBeta = 0;
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_ap_DBG_L_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currAlpha = 45;
		currBeta = -90;
		break;
	case AnyPointState::MODE_2:
		currAlpha = 0;
		currBeta = -45;
		break;
	}
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_ap_DBG_R_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currAlpha = 45;
		currBeta = 90;
		break;
	case AnyPointState::MODE_2:
		currAlpha = 0;
		currBeta = 45;
		break;
	}
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_ap_DBG_D_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currAlpha = -45;
		currBeta = 0;
		break;
	case AnyPointState::MODE_2:
		currAlpha = -45;
		currBeta = 0;
		break;
	}
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_ap_DBG_LL_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currAlpha = -45;
		currBeta = 45;
		break;
	case AnyPointState::MODE_2:
		currAlpha = -45;
		currBeta = -45;
		break;
	}
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_ap_DBG_LR_clicked()
{
	if (medicalState != MedicalState::ANYPOINT) return;
	switch (anypointState) {
	case AnyPointState::MODE_1:
		currAlpha = -45;
		currBeta = -45;
		break;
	case AnyPointState::MODE_2:
		currAlpha = -45;
		currBeta = 45;
		break;
	}
	doAnyPoint();
	DisplayOne(false);
}

void MainWindow::on_pushButton_Center_clicked()
{

	double Cx = ui->lineEdit_Cx->text().toDouble();
	double Cy = ui->lineEdit_Cy->text().toDouble();
	string Str = Para[currPara];   
    // json Para = json::parse(Str);
    QString str1 = QString::fromStdString(Str);
    QByteArray jsonData = str1.toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject Para = jsonDoc.object();
    md.Config(Para["cameraName"].toString().toStdString(), Para["cameraSensorWidth"].toDouble(), Para["cameraSensorHeight"].toDouble(),
        Para["iCx"].toDouble(), Para["iCy"].toDouble(), Para["ratio"].toDouble(),
        Para["imageWidth"].toDouble(), Para["imageHeight"].toDouble(), Para["calibrationRatio"].toDouble(),
        Para["parameter0"].toDouble(),
        Para["parameter1"].toDouble(),
        Para["parameter2"].toDouble(),
        Para["parameter3"].toDouble(),
        Para["parameter4"].toDouble(),
        Para["parameter5"].toDouble()
    );

	if (medicalState == MedicalState::ANYPOINT) {
		doAnyPoint();
		renderArea->setCenter(QPointF(md.getiCx() / md.getImageWidth(), md.getiCy() / md.getImageHeight()), RadiusF[currPara]);
	}
	DisplayOne(false);

}
