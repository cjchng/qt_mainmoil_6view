#ifndef LABEL_H
#define LABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <Qt>
#include <QMessageBox>
#include <QPoint>

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QPolygon>
#include <QPainter>

class Label : public QLabel
{
    Q_OBJECT
public:
    Label(const QString & text,QWidget *parent=0);
    ~Label();
    bool isDrawCenterEnabled = false;
private:
    QPoint Center;

protected:
virtual void mousePressEvent(QMouseEvent *event);
virtual void mouseReleaseEvent(QMouseEvent *event);
virtual void mouseDoubleClickEvent(QMouseEvent *event);
virtual void mouseMoveEvent(QMouseEvent *event);
virtual void wheelEvent ( QWheelEvent * event );
void paintEvent(QPaintEvent *event);

signals:
void pressed(QMouseEvent *event);
void clicked(QMouseEvent *event);
void doubled(QMouseEvent *event);
void moved(QMouseEvent *event);
void wheeled( QWheelEvent * event );

};

#endif // LABEL_H
