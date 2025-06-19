#include <QPainterPath>
#include "mlabel.h"
Label::Label(const QString & text,QWidget *parent):QLabel(text,parent)
{
    setText(text);
}
//Label::Label(QWidget *parent, Qt::WindowFlags f):QLabel(parent,f){}
//Label::Label(const QString &text, QWidget *parent, Qt::WindowFlags f):QLabel(text,parent,f){}
Label::~Label(){}

/*
bool Label::event(QEvent *ev)
{
    if(ev->type()==QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(ev);
        int x = mouseEvent->x();
        int y = mouseEvent->y();
        if(mouseEvent->button()==Qt::LeftButton)
        {
            QString str ;
            str.sprintf("%s %d %d","LeftButton", x, y);
            // QMessageBox::information(NULL,"Info", str, QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            return true;
        }

    }

    return QLabel::event(ev);
}
*/
void Label::mousePressEvent(QMouseEvent *event)
{
    emit pressed(event);
}

void Label::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(event);
}

void Label::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubled(event);
}

void Label::mouseMoveEvent(QMouseEvent *event)
{
    emit moved(event);
}

void Label::wheelEvent(QWheelEvent *event)
{
    emit wheeled(event);
}

void Label::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter painter(this);
    QPainterPath pathTopEdge;
    painter.setBrush(QBrush(Qt::green, Qt::LinearGradientPattern));
    Center.setX(this->width()/2);
    Center.setY(this->height()/2);
    int crossHair_Size = 20;
    QPen white_pen(Qt::white, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    QPen black_pen(Qt::black, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    if ( isDrawCenterEnabled ) {
        pathTopEdge = QPainterPath();
        pathTopEdge.moveTo(Center.x()-crossHair_Size+3,Center.y()+3);
        pathTopEdge.lineTo(Center.x()+crossHair_Size+3,Center.y()+3);
        pathTopEdge.moveTo(Center.x()+3,Center.y()-crossHair_Size+3);
        pathTopEdge.lineTo(Center.x()+3,Center.y()+crossHair_Size+3);
        painter.setPen(black_pen);
        painter.drawPath(pathTopEdge);
        pathTopEdge = QPainterPath();
        pathTopEdge.moveTo(Center.x()-crossHair_Size,Center.y());
        pathTopEdge.lineTo(Center.x()+crossHair_Size,Center.y());
        pathTopEdge.moveTo(Center.x(),Center.y()-crossHair_Size);
        pathTopEdge.lineTo(Center.x(),Center.y()+crossHair_Size);
        painter.setPen(white_pen);
        painter.drawPath(pathTopEdge);
    }

}


