#include "mainwindow.h"

#define MAP_CACHE_ENABLED true

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    MOIL_APP = MoilApp::CAR;                // Options : CAR, MEDICAL
    medicalState = MedicalState::ANYPOINT;  // Options : ORIGINAL, ANYPOINT, PANORAMA

    DIR* dir = opendir("snapshot");
    if (!dir)
    {
#ifdef _WIN32
    _mkdir("snapshot");
#else
    mkdir("snapshot",0755);
#endif
    }

    dbConnect("snapshot/moil.db");
    screen = QApplication::desktop()->screenGeometry();
    timer   = new QTimer(this);
    ui->setupUi(this);
    // this->move(0,screen.height()-this->geometry().height());
    this->setGeometry(QRect(QPoint(0, 0),QSize(screen.width(), screen.height())));
// repo220_T2
    md.Config("car", 1.4, 1.4,
        1320.0, 1017.0, 1.048,
        2592, 1944, 4.05,
        0, 0, 0, 0, -47.96, 222.86
        );
    double calibrationWidth = md.getImageWidth();
    double iCy = md.getiCy();
    ConfigData *cd = md.getcd();

    palWhite.setColor(QPalette::Button, QColor(Qt::white));
    palGray.setColor(QPalette::Button, QColor(Qt::lightGray));

// USB camera
// cap0 = cv::VideoCapture(0);

// MJPG-Streamer
// https://blog.miguelgrinberg.com/post/how-to-build-and-run-mjpg-streamer-on-the-raspberry-pi
// https://github.com/jacksonliam/mjpg-streamer
// const std::string videoStreamAddress = "http://192.168.100.2:8080/?action=stream";

// python3
// const std::string videoStreamAddress = "http://192.168.100.2:8000/stream.mjpg";
// cap0.open(videoStreamAddress);

    double w = fix_width;
    double h = fix_height;
// default input image
if ( MOIL_APP == MoilApp::CAR )
    readImage("images/image.jpg");
else if ( MOIL_APP == MoilApp::MEDICAL)
    readImage("images/image2.jpg");

    mapX_Medi = Mat(h, w, CV_32F);
    mapY_Medi = Mat(h, w, CV_32F);

    mapX_MediPano = Mat(h, w, CV_32F);
    mapY_MediPano = Mat(h, w, CV_32F);

    mapX[0] = Mat(h, w, CV_32F);
    mapX[1] = Mat(w, h, CV_32F);
    mapX[2] = Mat(w, h, CV_32F);
    mapY[0] = Mat(h, w, CV_32F);
    mapY[1] = Mat(w, h, CV_32F);
    mapY[2] = Mat(w, h, CV_32F);
    for (uint i=3;i<6;i++)
         mapX[i] = Mat(h, w, CV_32F);
    for (uint i=3;i<6;i++)
         mapY[i] = Mat(h, w, CV_32F);

    m_ratio = w / calibrationWidth;
    clock_t tStart = clock();
    char str_x[12], str_y[12];
    int i = 0;

if ( MOIL_APP == MoilApp::CAR )
{
if ( MAP_CACHE_ENABLED ) {

bool map_exist = true ;

while (map_exist && (i < 6)) {
    sprintf(str_x, "matX%d", i);sprintf(str_y, "matY%d", i);
    if ( !fopen(str_x, "r") || !fopen(str_y, "r"))
        map_exist = false ;
    i++;
}
if ( map_exist ) {
for (i=0;i<6;i++) {
    sprintf(str_x, "matX%d", i);sprintf(str_y, "matY%d", i);
    mapX[i] = MatRead(str_x);
    mapY[i] = MatRead(str_y);
}
}
else {
    md.AnyPointM((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 4, m_ratio);       // front view
    md.AnyPointM((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 70, 270, 4, m_ratio);   // left view, rotate 90
    md.AnyPointM((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 70, 90, 4, m_ratio);    // right view, rotate -90
    md.AnyPointM((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 4, m_ratio);      // Down view ( zoom: 2/4 )
    md.AnyPointM((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, 70, 225, 4, m_ratio);   // left-lower view, rotate 180
    md.AnyPointM((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, 70, 135, 4, m_ratio);   // right-lower view, rotate 180
for (i=0;i<6;i++) {
    sprintf(str_x, "matX%d", i);sprintf(str_y, "matY%d", i);
    MatWrite(str_x,mapX[i]);
    MatWrite(str_y,mapY[i]);
}

}

}
else {
    md.AnyPointM((float *)mapX[0].data, (float *)mapY[0].data, mapX[0].cols, mapX[0].rows, 0, 0, 4, m_ratio);       // front view
    md.AnyPointM((float *)mapX[1].data, (float *)mapY[1].data, mapX[1].cols, mapX[1].rows, 70, 270, 4, m_ratio);   // left view, rotate 90
    md.AnyPointM((float *)mapX[2].data, (float *)mapY[2].data, mapX[2].cols, mapX[2].rows, 70, 90, 4, m_ratio);    // right view, rotate -90
    md.AnyPointM((float *)mapX[3].data, (float *)mapY[3].data, mapX[3].cols, mapX[3].rows, -70, 0, 4, m_ratio);      // Down view ( zoom: 2/4 )
    md.AnyPointM((float *)mapX[4].data, (float *)mapY[4].data, mapX[4].cols, mapX[4].rows, 70, 225, 4, m_ratio);   // left-lower view, rotate 180
    md.AnyPointM((float *)mapX[5].data, (float *)mapY[5].data, mapX[5].cols, mapX[5].rows, 70, 135, 4, m_ratio);   // right-lower view, rotate 180
}
}
else // ( MOIL_APP == MoilApp::MEDICAL )
{
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, 4, m_ratio);
    md.Panorama(mapX_MediPano, mapY_MediPano, m_ratio);
}

// intel i7-8700
// resolution   : 2592 x 1944 = 5M pixels
// md.AnyPoint(): 0.458 sec.  (_m): 0.498 sec.
// md.Remap()   : 0.235 sec.
// md.Remap()+Rotate() : 0.323 sec.
// MatRad (X+Y) : 0.0157 sec. ( 1/1000 of AnyPoint)
// panorama : 0.36 sec. -> 0.28 without 2nd Mat

    double time_clock = (double)(clock() - tStart)/CLOCKS_PER_SEC ;
    main_width = this->geometry().width()-400 ;
    main_height = this->geometry().height()-300 ;

ch_width = (screen.width()-100)/3 ;
ch_height = ch_width*3/4 ;
int ch_limit = (screen.height() - 120 - y_gap - 40)/2 ;
ch_height = (ch_height > ch_limit)?ch_limit:ch_height;

// original image
// cv::resize(image_input, image_input_s, Size(ch_width,ch_height));



switch ( MOIL_APP ) {
    case MoilApp::CAR :
    ui->listWidget->hide();
    m_button_cam = new QPushButton("Camera", this);
    m_button_cam->setGeometry(QRect(QPoint( 30, screen.height()-120),QSize(100, 50)));
    connect(m_button_cam, SIGNAL (released()), this, SLOT (camButtonClicked()));

    m_button_multi = new QPushButton("Multi", this);
    m_button_multi->setGeometry(QRect(QPoint(160, screen.height()-120),QSize(80, 50)));

    for( int i=0;i<6;i++ ) {
        char str[10];
        sprintf(str, "%d", i+1);
        m_button_ch[i] = new QPushButton(str, this);
        m_button_ch[i]->setGeometry(QRect(QPoint(190+(i+1)*60, screen.height()-120),QSize(50, 50)));
    }

    connect(m_button_multi, SIGNAL (released()), this, SLOT (multiButtonClicked()));
    connect(m_button_ch[0], SIGNAL (released()), this, SLOT (ch1ButtonClicked()));
    connect(m_button_ch[1], SIGNAL (released()), this, SLOT (ch2ButtonClicked()));
    connect(m_button_ch[2], SIGNAL (released()), this, SLOT (ch3ButtonClicked()));
    connect(m_button_ch[3], SIGNAL (released()), this, SLOT (ch4ButtonClicked()));
    connect(m_button_ch[4], SIGNAL (released()), this, SLOT (ch5ButtonClicked()));
    connect(m_button_ch[5], SIGNAL (released()), this, SLOT (ch6ButtonClicked()));


        break;
    case MoilApp::MEDICAL :

    m_button_cam = new QPushButton("Camera", this);
    m_button_cam->setGeometry(QRect(QPoint( 40, screen.height()-190),QSize(100, 100)));
    m_button_cam->setStyleSheet("border:3px solid darkgray;");
    connect(m_button_cam, SIGNAL (released()), this, SLOT (camButtonClicked()));

    QPoint arrowsPos(520, screen.height()-156);
    QSize arrowSize(46,46);
    QPixmap *pixmap;

    // m_original = new QPushButton("", this);
    m_original = new Label("", this);
    m_original->setGeometry(QRect(arrowsPos + QPoint(-280,-45),QSize(220,140)));
    m_original->setStyleSheet("border:3px solid darkgray;");
    connect(m_original, SIGNAL (clicked(QMouseEvent*)), this, SLOT (originalClicked(QMouseEvent*)));

    m_anypoint = new QPushButton("", this);
    m_anypoint->setGeometry(QRect(arrowsPos + QPoint(-46,-45),QSize(220,140)));
    m_anypoint->setStyleSheet("border:3px solid darkgray;");
    m_anypoint->setShortcut(QKeySequence(Qt::Key_A));
    connect(m_anypoint, SIGNAL (released()), this, SLOT (anypointClicked()));

    m_paronama = new QPushButton("", this);
    m_paronama->setGeometry(QRect(arrowsPos + QPoint(200,-45),QSize(220,140)));
    pixmap = new QPixmap("images/pano.png");
    m_paronama->setIcon(QIcon(*pixmap));
    m_paronama->setIconSize(QSize(210,130));
    m_paronama->setStyleSheet("border:3px solid darkgray;");
    m_paronama->setShortcut(QKeySequence(Qt::Key_P));
    connect(m_paronama, SIGNAL (released()), this, SLOT (panoramaClicked()));

    m_left = new QPushButton("", this);
    m_left->setGeometry(QRect(arrowsPos,arrowSize));
    pixmap = new QPixmap("images/undo.svg");
    m_left->setIcon(QIcon(*pixmap));
    m_left->setIconSize(pixmap->rect().size());
    m_left->setStyleSheet("border:2px solid darkgray;");
    m_left->setShortcut(QKeySequence(Qt::Key_Left));
    connect(m_left, SIGNAL (released()), this, SLOT (leftClicked()));

    m_right = new QPushButton("", this);
    m_right->setGeometry(QRect(arrowsPos + QPoint(92,0),arrowSize));
    pixmap = new QPixmap("images/redo.svg");
    m_right->setIcon(QIcon(*pixmap));
    m_right->setIconSize(pixmap->rect().size());
    m_right->setStyleSheet("border:2px solid darkgray;");
    m_right->setShortcut(QKeySequence(Qt::Key_Right));
    connect(m_right, SIGNAL (released()), this, SLOT (rightClicked()));

    m_up = new QPushButton("", this);
    m_up->setGeometry(QRect(arrowsPos + QPoint(46,-44),arrowSize));
    pixmap = new QPixmap("images/arrow-up2.svg");
    m_up->setIcon(QIcon(*pixmap));
    m_up->setIconSize(pixmap->rect().size());
    m_up->setStyleSheet("border:2px solid darkgray;");
    m_up->setShortcut(QKeySequence(Qt::Key_Up));
    connect(m_up, SIGNAL (released()), this, SLOT (upClicked()));

    m_down = new QPushButton("", this);
    m_down->setGeometry(QRect(arrowsPos + QPoint(46,46),arrowSize));
    pixmap = new QPixmap("images/arrow-down2.svg");
    m_down->setIcon(QIcon(*pixmap));
    m_down->setIconSize(pixmap->rect().size());
    m_down->setStyleSheet("border:2px solid darkgray;");
    m_down->setShortcut(QKeySequence(Qt::Key_Down));
    connect(m_down, SIGNAL (released()), this, SLOT (downClicked()));

    m_reset = new QPushButton("", this);
    m_reset->setGeometry(QRect(arrowsPos + QPoint(48,0),arrowSize));
    pixmap = new QPixmap("images/radio-checked2.svg");
    m_reset->setIcon(QIcon(*pixmap));
    m_reset->setIconSize(pixmap->rect().size());
    m_reset->setStyleSheet("border: none;");
    m_reset->setShortcut(QKeySequence(Qt::Key_Space));
    connect(m_reset, SIGNAL (released()), this, SLOT (resetClicked()));

    pLabel = new Label("", this);
    pLabel->setGeometry(x_base,y_base,main_width,main_height);
    pLabel->stackUnder(m_button_cam);
    // pLabel->setStyleSheet("border:1px solid darkgray;");
    ui->label1->stackUnder(pLabel);

    connect(pLabel, SIGNAL(pressed(QMouseEvent*)), this, SLOT(onMousePressed(QMouseEvent*)));
    connect(pLabel, SIGNAL(clicked(QMouseEvent*)), this, SLOT(onMouseReleased(QMouseEvent*)));
    connect(pLabel, SIGNAL(moved(QMouseEvent*)), this, SLOT(onMouseMoved(QMouseEvent*)));
    connect(pLabel, SIGNAL(wheeled(QWheelEvent*)), this, SLOT(onWheel(QWheelEvent*)));

    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onListItemClicked(QListWidgetItem*)));

 ui->listWidget->setGeometry(QRect(QPoint(this->geometry().width()-350, 25),QSize( 240, this->geometry().height()-180)));
 ui->listWidget->setToolTip("Image List");
 ui->listWidget->setIconSize(QSize(240,180));
 ui->listWidget->setViewMode(QListView::IconMode);
 pLabel->stackUnder(ui->listWidget);
 reSnapshotList();


        break;
}
        connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));

        QToolBar *toolBar = addToolBar(tr("&File"));

        openParaAction = new QAction(QIcon("./images/file-text2.svg"), tr("&Load Parameter"), this);
        toolBar->addAction(openParaAction);
        connect(openParaAction, SIGNAL(triggered()), this, SLOT (openParameter()));

        openImageAction = new QAction(QIcon("./images/image.svg"), tr("&Load Image"), this);
        toolBar->addAction(openImageAction);
        connect(openImageAction, SIGNAL(triggered()), this, SLOT (openImage()));

        QToolBar *toolBar2 = addToolBar(tr("&Bar2"));
        snapAction = new QAction(QIcon("./images/camera.svg"), tr("&Take Snapshot"), this);    
        QWidget *spacerWidget = new QWidget(this);
        spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        spacerWidget->setVisible(true);
        toolBar2->addWidget(spacerWidget);
        toolBar2->addAction(snapAction);

        connect(snapAction, SIGNAL(triggered()), this, SLOT (snapshot()));

        m_pLabel = new QLabel("Camera URL :", this);
        m_pLabel->setGeometry(300,25, 100,30);
        m_pEdit = new QLineEdit("", this);
        m_pEdit->setGeometry(400,25, 400,30);

        m_pMsg = new QLabel("", this);
        m_pMsg->setGeometry(1200,25, 300,30);

        // m_pButton = new QPushButton("Save", this);
        // m_pButton->setGeometry(500,25, 30,30);
        // connect(m_pButton, SIGNAL (released()),this, SLOT (saveURL()));

        m_sSettingsFile = QApplication::applicationDirPath() + "/settings.ini";
        loadSettings();
        switch ( MOIL_APP ) {
            case MoilApp::CAR :
            DisplayCh(0);
            break;
            case MoilApp::MEDICAL :
            DisplayOne(true);
        }
}


void MainWindow::showMoilInfo()
{
    QString str;
    if ( MOIL_APP == MoilApp::MEDICAL )
    {
        if ( medicalState == MedicalState::ANYPOINT )
            str.sprintf("A: %d     B: %d    Z: %.2f", currAlpha, currBeta, currZoom);
        else
            str = "";
        if (m_pMsg!=NULL)
        m_pMsg->setText(str);
    }
}

void MainWindow::readImage(QString filename)
{
    std::string fname = filename.toUtf8().constData();
    image_input = imread( fname, IMREAD_COLOR);
    if( !image_input.empty() )
        {
        if (( image_input.cols != fix_width ) || ( image_input.rows != fix_height )) {
        cv::resize(image_input, image_input, Size(fix_width, fix_height));
        }
        Vec3b p(0,0,0) ;
        image_input.at<Vec3b>(0, 1) = p;
    }
}

void MainWindow::snapshot()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char f_name[32], filename[64];
    sprintf(f_name,"%04d%02d%02d_%02d%02d%02d.jpg",ltm->tm_year+1900,ltm->tm_mon+1,ltm->tm_mday,ltm->tm_hour,ltm->tm_min,ltm->tm_sec );
    sprintf(filename,"snapshot/%s", f_name);
    DIR* dir = opendir("snapshot");
    if (!dir)
    {
#ifdef _WIN32
    _mkdir("snapshot");
#else
    mkdir("snapshot",0755);
#endif
    }


    switch ( MOIL_APP ) {
        case MoilApp::CAR :
Mat *thisMat;
switch ( currCh ) {
case 2:
    thisMat = &image_display[0];
    break;
case 1:
    thisMat = &image_display[1];
    break;
case 3: case 4: case 5: case 6:
    thisMat = &image_display[currCh-1];
    break;
}
if (!thisMat->empty()) {
    Mat image_display_tmp;
    cvtColor(*thisMat, image_display_tmp, CV_RGB2BGR);
    cv::imwrite(filename, image_display_tmp);
    reSnapshotList();
}

        break;
        case MoilApp::MEDICAL :

    if (!image_display[0].empty()) {

        // fisheye image
        Mat image_display_tmp;
        cv::imwrite(filename, image_input);

        // thumbnail
        sprintf(filename,"snapshot/thumbnail/%s", f_name);
        DIR* dir2 = opendir("snapshot/thumbnail");
        if (!dir2)
        {
    #ifdef _WIN32
        _mkdir("snapshot/thumbnail");
    #else
        mkdir("snapshot/thumbnail",0755);
    #endif
        }
        cvtColor(image_display[0], image_display_tmp, CV_RGB2BGR);
        cv::imwrite(filename, image_display_tmp);
        QString str(f_name);
        dbAddRecord(str, currAlpha, currBeta, currZoom);

        reSnapshotList();
    }
}

    closedir(dir);
}



void MainWindow::reSnapshotList()
{
    dirent *dp ;

    DIR *dir = opendir("snapshot");

    if (dir) {
    ui->listWidget->clear();
    while ((dp = readdir(dir)) != NULL)
    {
        QString str(dp->d_name);
        if (str.contains(".jpg"))
        {
        ui->listWidget->addItem(new QListWidgetItem(
                           QIcon("snapshot/thumbnail/" + str), str));
        qDebug( "%s", dp->d_name );
        }
    }
    ui->listWidget->sortItems();
    closedir(dir);
    }
}
void MainWindow::DisplayCh(int ch)
{
Mat image_result, image_resultv;
if( image_input.empty()) return ;

    switch (ch) {
    case 0:  // 2 x 3
        // md.Remap(image_input, image_result, mapX[0], mapY[0]);
        remap( image_input, image_result, mapX[0], mapY[0], INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
        cv::resize(image_result, image_display[0], Size(ch_width,ch_height));
        cvtColor(image_display[0], image_display[0], CV_BGR2RGB);
        DisplayWindow(image_display[0], ui->label2, x_base+ch_width, y_base, ch_width, ch_height-y_base);

        md.Remap(image_input, image_resultv, mapX[1], mapY[1]);
        Rotate(image_resultv, image_result, 90.0);
        cv::resize(image_result, image_display[1], Size(ch_width,ch_height));
        cvtColor(image_display[1], image_display[1], CV_BGR2RGB);
        DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width,ch_height-y_base);

        md.Remap(image_input, image_resultv, mapX[2], mapY[2]);
        Rotate(image_resultv, image_result, -90.0);
        cv::resize(image_result, image_display[2], Size(ch_width,ch_height));
        cvtColor(image_display[2], image_display[2], CV_BGR2RGB);
        DisplayWindow(image_display[2], ui->label3, x_base+ch_width*2, y_base, ch_width,ch_height-y_base);

        md.Remap(image_input, image_result, mapX[3], mapY[3]);
        cv::resize(image_result, image_display[3], Size(ch_width,ch_height));
        cvtColor(image_display[3], image_display[3], CV_BGR2RGB);
        DisplayWindow(image_display[3], ui->label4, x_base+ch_width, ch_height+y_gap, ch_width,ch_height-y_base);

        md.Remap(image_input, image_result, mapX[4], mapY[4]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[4], Size(ch_width,ch_height));
        cvtColor(image_display[4], image_display[4], CV_BGR2RGB);
        DisplayWindow(image_display[4], ui->label5, x_base, ch_height+y_gap, ch_width,ch_height-y_base);

        md.Remap(image_input, image_result, mapX[5], mapY[5]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[5], Size(ch_width,ch_height));
        cvtColor(image_display[5], image_display[5], CV_BGR2RGB);
        DisplayWindow(image_display[5], ui->label6, x_base+ch_width*2, ch_height+y_gap, ch_width,ch_height-y_base);
        break;
    case 2:
        md.Remap(image_input, image_result, mapX[0], mapY[0]);
        cv::resize(image_result, image_display[0], Size(ch_width*3-60,ch_height*2-45));
        cvtColor(image_display[0], image_display[0], CV_BGR2RGB);
        DisplayWindow(image_display[0], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    case 1:
        md.Remap(image_input, image_resultv, mapX[1], mapY[1]);
        Rotate(image_resultv, image_result, 90.0);
        cv::resize(image_result, image_display[1], Size(ch_width*3*0.9,ch_height*2*0.9));
        cvtColor(image_display[1], image_display[1], CV_BGR2RGB);
        DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    case 3:
        md.Remap(image_input, image_resultv, mapX[2], mapY[2]);
        Rotate(image_resultv, image_result, -90.0);
        cv::resize(image_result, image_display[2], Size(ch_width*3*0.9,ch_height*2*0.9));
        cvtColor(image_display[2], image_display[2], CV_BGR2RGB);
        DisplayWindow(image_display[2], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    case 4:
        md.Remap(image_input, image_result, mapX[3], mapY[3]);
        cv::resize(image_result, image_display[3], Size(ch_width*3*0.9,ch_height*2*0.9));
        cvtColor(image_display[3], image_display[3], CV_BGR2RGB);
        DisplayWindow(image_display[3], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    case 5:
        md.Remap(image_input, image_result, mapX[4], mapY[4]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[4], Size(ch_width*3*0.9,ch_height*2*0.9));
        cvtColor(image_display[4], image_display[4], CV_BGR2RGB);
        DisplayWindow(image_display[4], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    case 6:
        md.Remap(image_input, image_result, mapX[5], mapY[5]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[5], Size(ch_width*3*0.9,ch_height*2*0.9));
        cvtColor(image_display[5], image_display[5], CV_BGR2RGB);
        DisplayWindow(image_display[5], ui->label1, x_base, y_base, ch_width*3*0.9,ch_height*2*0.9);
        break;
    }
    if (ch==0) {
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
    for (int i=0;i<6;i++)
    m_button_ch[i]->setPalette(palWhite);
    if( ch==0 )
        m_button_multi->setPalette(palGray);
    else
        m_button_ch[ch-1]->setPalette(palGray);

    currCh = ch ;


}

void MainWindow::DisplayOne(bool refreshOriginal)
{
Mat image_result;
if( image_input.empty()) return ;
Mat image_original ;

if (refreshOriginal) {
cv::resize(image_input,  image_original, Size(m_original->geometry().width(), m_original->geometry().height()));
cvtColor(image_original, image_original, CV_BGR2RGB);
DisplayWindow(image_original, m_original, m_original->geometry().x(), m_original->geometry().y(), m_original->geometry().width(), m_original->geometry().height());
}

switch ( medicalState ) {
case MedicalState::ORIGINAL :
    cv::resize(image_input,  image_display[0], Size(main_width,main_height));
    break;
case MedicalState::ANYPOINT :
    md.Remap(image_input, image_result, mapX_Medi, mapY_Medi);
    cv::resize(image_result,  image_display[0], Size(main_width,main_height));
    break;
case MedicalState::PANORAMA :
    md.Remap(image_input, image_result, mapX_MediPano, mapY_MediPano);
    cv::resize(image_result,  image_display[0], Size(main_width,main_height));
    break;

}
    cvtColor(image_display[0], image_display[0], CV_BGR2RGB);
    DisplayWindow(image_display[0], ui->label1, x_base, y_base, main_width,main_height );
    showMoilInfo();
}



void MainWindow::DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h)
{
    QImage q_image1= QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    p_label->setPixmap(QPixmap::fromImage(q_image1));
    p_label->setStyleSheet("border:1px solid black;");
    p_label->setGeometry(x,y,w,h);
}

void MainWindow::Rotate(Mat& src, Mat& dst, double angle)
{
        Point2f center((src.cols-1)/2.0, (src.rows-1)/2.0);
        Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
        Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();
        rot.at<double>(0,2) += bbox.width/2.0 - src.cols/2.0;
        rot.at<double>(1,2) += bbox.height/2.0 - src.rows/2.0;
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
    if ( !CaptureState ) {
        openCamera();
    }
    else {
        closeCamera();
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
    QString str ;
    prevMousePos = event->pos();
}


void MainWindow::onMouseReleased(QMouseEvent *event)
{
    mouseState = 0;
}

void MainWindow::onMouseMoved(QMouseEvent *event)
{
if(medicalState != MedicalState::ANYPOINT) return ;
if (mouseState == 0)
{
    prevMousePos = event->pos();
    mouseState = 1;
    return ;
}
QString str ;
QPoint Offset = event->pos() - prevMousePos;

    int x_inc = Offset.x()/2;
    int y_inc = Offset.y()/2;

if (((currAlpha - y_inc) >= -90) && ((currAlpha - y_inc) <= 90))
      currAlpha = currAlpha - y_inc ;
    else if ((currAlpha - y_inc) > 90)
      currAlpha = 90;
    else
      currAlpha = -90;

    currBeta = (currBeta + x_inc) % 360;
    if ( currBeta < 0 )
         currBeta += 360 ;
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);


    prevMousePos = event->pos();
}

void MainWindow::onWheel(QWheelEvent *event)
{
if(medicalState != MedicalState::ANYPOINT) return ;

    QString str ;
    double w = (double)event->delta()/200.0;
    if ( w > 0 ) {
        if ( currZoom + w < maxZoom )
            currZoom += w ;
        else
            currZoom = maxZoom ;
    }
    else if ( w < 0 ) {
        if ( currZoom + w >= minZoom )
            currZoom += w ;
        else
            currZoom = minZoom;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);

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
    const std::string videoStreamAddress = m_pEdit->text().toUtf8().constData() ;
    cap0.open(videoStreamAddress);
    if ( cap0.isOpened() ) {
    timer->start(200);
    CaptureState = true ;
    m_button_cam->setStyleSheet("border:5px solid darkgray;background-color: lightgray;");
    if ( MOIL_APP == MoilApp::MEDICAL )
        medicalState = MedicalState::ANYPOINT ;
    }
}



void MainWindow::closeCamera()
{
    CaptureState = false ;
    m_button_cam->setStyleSheet("border:3px solid darkgray;background-color: white;");
    timer->stop();
    cap0.release();
}

void MainWindow::readFrame()
{
    if ( cap0.isOpened() ) {
    cap0.read(image_input);
    switch ( MOIL_APP ) {
        case MoilApp::CAR :
            DisplayCh(currCh);
        break;
    case MoilApp::MEDICAL :
            DisplayOne(true);
        break;
    }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImage()
{
    bool prevCamActive = false ;
    if ( cap0.isOpened() ) {
    closeCamera();
    prevCamActive = true ;
    }

    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
    if ( !filename.isEmpty() ) {
        readImage(filename);
        switch ( MOIL_APP ) {
            case MoilApp::CAR :
            DisplayCh(currCh);
            break;
            case MoilApp::MEDICAL :
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
            resetClicked();
        }
    }
    else {
        if( prevCamActive ) {
            openCamera();
        }
    }
}

void MainWindow::openParameter()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Parameters"), ".", tr("Json Files (*.json)"));
    if(!filename.isEmpty()) {
        std::string fname = filename.toUtf8().constData();
        ifstream inFile;
        inFile.open(fname);

        stringstream strStream;
        strStream << inFile.rdbuf(); // Read the file
        string string = strStream.str();
        json Para = json::parse(string);
        md.Config("car", Para["cameraSensorWidth"], Para["cameraSensorHeight"],
            Para["iCx"], Para["iCy"], Para["ratio"],
            Para["imageWidth"], Para["imageHeight"], Para["calibrationRatio"],
            Para["parameter0"],
            Para["parameter1"],
            Para["parameter2"],
            Para["parameter3"],
            Para["parameter4"],
            Para["parameter5"]
            );

        QMessageBox msgBox;
        QString AA = QString::fromStdString(string);
        msgBox.setText(AA);
        msgBox.exec();
    }
}

void MainWindow::on_actionLoad_Image_triggered()
{
    openImage();
}

void MainWindow::on_actionExit_triggered()
{
    saveSettings();
    QCoreApplication::exit() ;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
}

void MainWindow::on_actionMOIL_triggered()
{
    // QMessageBox::information(NULL,"About","MOIL \n\n\n Ming Chi University of Technology",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);

    QMessageBox msgBox;
    msgBox.setText("MOIL \n\n\n Omnidirectional Imaging & Survaillance Lab");
    msgBox.setIconPixmap(QPixmap("images/chess.jpg"));
    msgBox.exec();

    /*
    QDialog *subDialog = new QDialog;
    subDialog->setWindowTitle("MOIL");
    subDialog->setStyleSheet( "background-image:url(images/image.jpg);background-size: 100px 100px;" );
    subDialog->show();
    */

}

void MainWindow::on_actionLoad_triggered()
{
    openParameter();
}


void MainWindow::loadSettings()
{
 QSettings settings(m_sSettingsFile, QSettings::NativeFormat);
 QString sText = settings.value("videoSourceURL", "").toString();
 if (sText == "")
     sText = QString::fromStdString( default_videoStreamURL );

 if (m_pEdit)
     m_pEdit->setText(sText);
}

void MainWindow::saveSettings()
{
 QSettings settings(m_sSettingsFile, QSettings::NativeFormat);
 QString sText = (m_pEdit) ? m_pEdit->text() : "";
 settings.setValue("videoSourceURL", sText);
}

void MainWindow::saveURL()
{
 saveSettings();
}

void MainWindow::on_actionVideo_Source_triggered()
{
 openParameter();
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
    int a, b;
    float z;
    bool Ret = dbGetRecord(filename, &a, &b, &z) ;
    if ( Ret ) {
    if ( cap0.isOpened() ) {
    closeCamera();
    }

    readImage("snapshot/" + filename);

    switch ( MOIL_APP ) {
        case MoilApp::CAR :
        DisplayCh(currCh);
        break;
        case MoilApp::MEDICAL :
        medicalState = MedicalState::ANYPOINT ;
        refreshMedicalState();
        currAlpha = a;
        currBeta = b;
        currZoom = z;
        md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
        DisplayOne(true);
    }



    }
}

void MainWindow::upClicked()
{
    if(medicalState != MedicalState::ANYPOINT) {
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
    }
    else {
    if ( currAlpha + currInc <= 90 )
        currAlpha = currAlpha + currInc ;
    else
        currAlpha = 90 ;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);
}

void MainWindow::downClicked()
{
    if(medicalState != MedicalState::ANYPOINT) {
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
    }
    else {
    if ( currAlpha - currInc >= -90 )
        currAlpha = currAlpha - currInc ;
    else
        currAlpha = -90 ;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);
}

void MainWindow::leftClicked()
{
    if(medicalState != MedicalState::ANYPOINT) {
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
    }
    else {
    if( currBeta - currInc >= 0 )
        currBeta = currBeta - currInc ;
    else
        currBeta = ( currBeta - currInc ) + 360 ;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);
}

void MainWindow::rightClicked()
{
    if(medicalState != MedicalState::ANYPOINT) {
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
    }
    else {
    currBeta = ( currBeta + currInc ) % 360 ;
    if (currBeta < 0) currBeta += 360;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);
}

void MainWindow::refreshMedicalState()
{
    switch( medicalState ) {
    case MedicalState::ORIGINAL :
        m_original->setStyleSheet("border:5px solid red;");
        m_anypoint->setStyleSheet("border:3px solid darkgray;");
        m_paronama->setStyleSheet("border:3px solid darkgray;");
        break;
    case MedicalState::ANYPOINT :
        m_original->setStyleSheet("border:5px solid darkgray;");
        m_anypoint->setStyleSheet("border:3px solid red;");
        m_paronama->setStyleSheet("border:3px solid darkgray;");
        break;
    case MedicalState::PANORAMA :
        m_original->setStyleSheet("border:5px solid darkgray;");
        m_anypoint->setStyleSheet("border:3px solid darkgray;");
        m_paronama->setStyleSheet("border:3px solid red;");
        break;

    }
}
void MainWindow::resetClicked()
{
    if(medicalState != MedicalState::ANYPOINT) {
            medicalState = MedicalState::ANYPOINT ;
            refreshMedicalState();
    }
    else {
    currAlpha = 0;
    currBeta = 0;
    currZoom = defaultZoom ;
    }
    md.AnyPointM((float *)mapX_Medi.data, (float *)mapY_Medi.data, mapX_Medi.cols, mapX_Medi.rows, (double)currAlpha, (double)currBeta, currZoom, m_ratio);
    DisplayOne(false);
}


void MainWindow::originalClicked(QMouseEvent*)
{
    medicalState = MedicalState::ORIGINAL ;
    refreshMedicalState();
    DisplayOne(false);
}

void MainWindow::anypointClicked()
{
    medicalState = MedicalState::ANYPOINT ;
    refreshMedicalState();
    DisplayOne(false);
}

void MainWindow::panoramaClicked()
{
    medicalState = MedicalState::PANORAMA ;
    refreshMedicalState();
    DisplayOne(false);
}


bool MainWindow::dbConnect(const QString &dbName)
{
db = QSqlDatabase::addDatabase("QSQLITE");
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
"zoom FLOAT)")) {
return false;
}

return true;
}

bool MainWindow::dbAddRecord(QString filename, int alpha, int beta, float zoom)
{
    QSqlQuery query;
    QString str ;
    str.sprintf("INSERT INTO imageinfo (filename, alpha, beta, zoom) VALUES (\"%s\", %d, %d, %f)", filename.toUtf8().constData(), currAlpha, currBeta, currZoom);
    if(!query.exec(str)){
    return false ;
    }
    return true;
}

bool MainWindow::dbGetRecord(QString filename, int *alpha, int *beta, float *zoom)
{
    QSqlQuery query;
    QString str ;
    str.sprintf("SELECT * FROM imageinfo  WHERE filename =\"%s\"", filename.toUtf8().constData());
    if(!query.exec(str)){
    qDebug() << "Failed!";
    return false;
    }
    else {
        while(query.next())
        {
           qDebug() << query.value(0).toInt() << query.value(1).toString() << query.value(2).toInt() << query.value(3).toInt() << query.value(4).toFloat();
           *alpha = query.value(2).toInt();
           *beta = query.value(3).toInt();
           *zoom = query.value(4).toFloat();
        }
    }
return true;
}
