#include "mainwindow.h"

#define MAP_CACHE_ENABLED true
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    screen = QApplication::desktop()->screenGeometry();
    timer   = new QTimer(this);
    ui->setupUi(this);
    this->move(0,screen.height()-this->geometry().height());
// repo220_T2
    md.Config("car", 1.4, 1.4,
        1320.0, 1017.0, 1.048,
        2592, 1944, 4.05,
        0, 0, 0, 0, -47.96, 222.86
        );
    double calibrationWidth = md.getImageWidth();
    double iCy = md.getiCy();
    ConfigData *cd = md.getcd();

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
if (!cap0.isOpened())  // if not success, exit program
{
    QMessageBox msgBox;
    msgBox.setText("Cannot open the video cam");
    msgBox.exec();
    close();
    qApp->quit();
}
double dWidth = cap0.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
double dHeight = cap0.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
*/

    double w = fix_width;
    double h = fix_height;

    readImage("image.jpg");  // default input image

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

    double m_ratio = w / calibrationWidth;
    clock_t tStart = clock();
    char str_x[12], str_y[12];
    int i = 0;
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

// intel i7-8700
// resolution   : 2592 x 1944 = 5M pixels
// md.AnyPoint(): 0.458 sec.  (_m): 0.498 sec.
// md.Remap()   : 0.235 sec.
// md.Remap()+Rotate() : 0.323 sec.
// MatRad (X+Y) : 0.0157 sec. ( 1/1000 of AnyPoint)
// parorama : 0.36 sec. -> 0.28 without 2nd Mat

    double time_clock = (double)(clock() - tStart)/CLOCKS_PER_SEC ;


ch_width = (screen.width()-100)/3 ;
ch_height = ch_width*3/4 ;

// original image
// cv::resize(image_input, image_input_s, Size(ch_width,ch_height));

    DisplayCh(0);

    this->setGeometry(QRect(QPoint(0, 0),QSize(screen.width(), screen.height())));

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


        connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));

        openAction = new QAction(QIcon("./images/image.svg"), tr("&Open Image"), this);
        QToolBar *toolBar = addToolBar(tr("&File"));
        toolBar->addAction(openAction);
        connect(openAction, SIGNAL(triggered()), this, SLOT (openImage()));

        m_pLabel = new QLabel("URL : ", this);
        m_pLabel->setGeometry(150,25, 100,30);
        m_pEdit = new QLineEdit("", this);
        m_pEdit->setGeometry(200,25, 300,30);
        // m_pButton = new QPushButton("Save", this);
        // m_pButton->setGeometry(500,25, 30,30);
        // connect(m_pButton, SIGNAL (released()),this, SLOT (saveURL()));
        m_sSettingsFile = QApplication::applicationDirPath() + "/settings.ini";
        loadSettings();

        connect(m_button_multi, SIGNAL (released()), this, SLOT (multiButtonClicked()));
        connect(m_button_ch[0], SIGNAL (released()), this, SLOT (ch1ButtonClicked()));
        connect(m_button_ch[1], SIGNAL (released()), this, SLOT (ch2ButtonClicked()));
        connect(m_button_ch[2], SIGNAL (released()), this, SLOT (ch3ButtonClicked()));
        connect(m_button_ch[3], SIGNAL (released()), this, SLOT (ch4ButtonClicked()));
        connect(m_button_ch[4], SIGNAL (released()), this, SLOT (ch5ButtonClicked()));
        connect(m_button_ch[5], SIGNAL (released()), this, SLOT (ch6ButtonClicked()));

        connect(ui->label1, SIGNAL (released()), this, SLOT (ch1ButtonClicked()));
        connect(ui->label2, SIGNAL (clicked()), this, SLOT (ch2ButtonClicked()));
        connect(ui->label3, SIGNAL (clicked()), this, SLOT (ch3ButtonClicked()));
        connect(ui->label4, SIGNAL (clicked()), this, SLOT (ch4ButtonClicked()));
        connect(ui->label5, SIGNAL (clicked()), this, SLOT (ch5ButtonClicked()));
        connect(ui->label6, SIGNAL (clicked()), this, SLOT (ch6ButtonClicked()));


}

void MainWindow::readImage(QString filename)
{
    std::string fname = filename.toUtf8().constData();
    image_input = imread( fname, IMREAD_COLOR);
    if( !image_input.empty() )                      // Check for invalid input
        {
        if (( image_input.cols != fix_width ) || ( image_input.rows != fix_height )) {
        cv::resize(image_input, image_input, Size(fix_width, fix_height));
        }
        Vec3b p(0,0,0) ;
        image_input.at<Vec3b>(0, 0) = p;
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
    }
    else
    {
        ui->label2->setVisible(false);
        ui->label3->setVisible(false);
        ui->label4->setVisible(false);
        ui->label5->setVisible(false);
        ui->label6->setVisible(false);
    }
    currCh = ch ;


}

void MainWindow::DisplayWindow(Mat& src, QLabel *p_label, int x, int y, int w, int h)
{
    // cvtColor(src, src, CV_BGR2RGB);
    QImage q_image1= QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    p_label->setPixmap(QPixmap::fromImage(q_image1));
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
        CaptureState = true ;
        openCamera();
    }
    else {
        closeCamera();
        CaptureState = false ;
    }
}
void MainWindow::multiButtonClicked()
{
    DisplayCh(0);
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
    const std::string videoStreamAddress = default_videoStreamURL ;
    cap0.open(videoStreamAddress);
    if ( cap0.isOpened() )
    timer->start(200);

}

void MainWindow::readFrame()
{
    if ( cap0.isOpened() ) {
    cap0.read(image_input);
    DisplayCh(currCh);
    }
}



void MainWindow::closeCamera()
{
    timer->stop();
    cap0.release();
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
        DisplayCh(currCh);
    }
    else {
        if( prevCamActive ) {
            openCamera();
        }
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
    QMessageBox msgBox;
    msgBox.setText("MOIL \n\n\n Ming Chi University of Technology");
    msgBox.exec();
}

void MainWindow::on_actionLoad_triggered()
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

}
