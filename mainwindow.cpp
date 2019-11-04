#include "mainwindow.h"

#define MAP_CACHE_ENABLED true
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    timer   = new QTimer(this);
    imag    = new QImage();
    frame   = new Mat;
    Moildev md ;
    screen = QApplication::desktop()->screenGeometry();

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
    image_input = imread( "image.jpg", IMREAD_COLOR);
    double w = image_input.cols;
    double h = image_input.rows;


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
    Vec3b p(0,0,0) ;
    image_input.at<Vec3b>(0, 0) = p;

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

void MainWindow::DisplayCh(int ch)
{
    Mat image_result, image_resultv;

    switch (ch) {
    case 0:  // 2 x 3
        md.Remap(image_input, image_result, mapX[0], mapY[0]);
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
        DisplayWindow(image_display[3], ui->label4, x_base+ch_width, ch_height+y_base, ch_width,ch_height-y_base);

        md.Remap(image_input, image_result, mapX[4], mapY[4]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[4], Size(ch_width,ch_height));
        cvtColor(image_display[4], image_display[4], CV_BGR2RGB);
        DisplayWindow(image_display[4], ui->label5, x_base, ch_height+y_base, ch_width,ch_height-y_base);

        md.Remap(image_input, image_result, mapX[5], mapY[5]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[5], Size(ch_width,ch_height));
        cvtColor(image_display[5], image_display[5], CV_BGR2RGB);
        DisplayWindow(image_display[5], ui->label6, x_base+ch_width*2, ch_height+y_base, ch_width,ch_height-y_base);
        break;
    case 2:
        md.Remap(image_input, image_result, mapX[0], mapY[0]);
        cv::resize(image_result, image_display[0], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[0], image_display[0], CV_BGR2RGB);
        DisplayWindow(image_display[0], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
        break;
    case 1:
        md.Remap(image_input, image_resultv, mapX[1], mapY[1]);
        Rotate(image_resultv, image_result, 90.0);
        cv::resize(image_result, image_display[1], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[1], image_display[1], CV_BGR2RGB);
        DisplayWindow(image_display[1], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
        break;
    case 3:
        md.Remap(image_input, image_resultv, mapX[2], mapY[2]);
        Rotate(image_resultv, image_result, -90.0);
        cv::resize(image_result, image_display[2], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[2], image_display[2], CV_BGR2RGB);
        DisplayWindow(image_display[2], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
        break;
    case 4:
        md.Remap(image_input, image_result, mapX[3], mapY[3]);
        cv::resize(image_result, image_display[3], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[3], image_display[3], CV_BGR2RGB);
        DisplayWindow(image_display[3], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
        break;
    case 5:
        md.Remap(image_input, image_result, mapX[4], mapY[4]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[4], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[4], image_display[4], CV_BGR2RGB);
        DisplayWindow(image_display[4], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
        break;
    case 6:
        md.Remap(image_input, image_result, mapX[5], mapY[5]);
        md.Rotate(image_result, image_result, 180.0);
        cv::resize(image_result, image_display[5], Size(ch_width*3-x_base,ch_height*2-y_base));
        cvtColor(image_display[5], image_display[5], CV_BGR2RGB);
        DisplayWindow(image_display[5], ui->label1, x_base, y_base, ch_width*3-x_base,ch_height*2-y_base);
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
        openCamara();
    }
    else {
        closeCamara();
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

void MainWindow::openCamara()
{
    timer->start(33);
}

void MainWindow::readFarme()
{
    if ( cap0->isOpened() ) {
    cap0->read(*frame);
    QImage image1= QImage((uchar*) frame->data, frame->cols, frame->rows, frame->step, QImage::Format_RGB888);
    ui->label1->setPixmap(QPixmap::fromImage(image1));
    }
    /*
    frame = cvQueryFrame(cam);
    QImage image((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(image));
    */
}

void MainWindow::takingPictures()
{
    /*
    frame = cvQueryFrame(cam);
    QImage image((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    ui->label1->setPixmap(QPixmap::fromImage(image));
    */
}


void MainWindow::closeCamara()
{
    timer->stop();
    //cvReleaseCapture(&cam);
}


MainWindow::~MainWindow()
{
    delete ui;
}
