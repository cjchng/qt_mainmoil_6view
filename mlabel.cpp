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
