#ifndef LABEL_H
#define LABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <Qt>
#include <QMessageBox>
#include <QPoint>
class Label : public QLabel
{
    Q_OBJECT
public:
    Label(const QString & text,QWidget *parent=0);
    //Label(QWidget *parent = 0, Qt::WindowFlags f=0);
    //Label(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
    ~Label();
    // virtual bool event(QEvent *event) override;

protected:
virtual void mousePressEvent(QMouseEvent *event);
virtual void mouseReleaseEvent(QMouseEvent *event);
virtual void mouseDoubleClickEvent(QMouseEvent *event);
virtual void mouseMoveEvent(QMouseEvent *event);
virtual void wheelEvent ( QWheelEvent * event );
signals:

void pressed(QMouseEvent *event);
void clicked(QMouseEvent *event);
void doubled(QMouseEvent *event);
void moved(QMouseEvent *event);
void wheeled( QWheelEvent * event );

};

#endif // LABEL_H
