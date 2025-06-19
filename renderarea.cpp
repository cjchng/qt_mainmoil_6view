/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderarea.h"
#include <QPainterPath>
#include <QPainter>
RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    CenterF.setX(0.5);
    CenterF.setY(0.5);
    Center.setX(this->width()*CenterF.x());
    Center.setY(this->height()*CenterF.y());
    RadiusF = 0.5;
    Radius = this->height()*RadiusF ;
    currPos.setX(Center.x());
    currPos.setY(Center.y());
    currSize = 30;
    currAlpha = currBeta = 0;
}

void RenderArea::setCenter(QPointF center, float r)
{
    CenterF = center ;
    RadiusF = r;
    Center.setX(this->width()*CenterF.x());
    Center.setY(this->height()*CenterF.y());
    Radius = this->height()*RadiusF ;
    update();
}

void RenderArea::setPos(QPoint pos)
{
    currPos = pos ;
    update();
}

QPointF RenderArea::getPosF()
{
    QPointF currPosF;
    currPosF.setX(currPos.x()/this->width());
    currPosF.setX(currPos.y()/this->height());
    return currPosF;
}

void RenderArea::setPos(int alpha, int beta)
{
    QPoint pos;
    currAlpha = alpha;
    currBeta = beta;
    beta = 90 - beta ;
    double r = (double)alpha * this->height() / 90 * RadiusF ;
    pos.setX(Center.x() + r*cos(3.14159265*(double)beta/180));
    pos.setY(Center.y() - r*sin(3.14159265*(double)beta/180));
    currPos = pos ;
    update();
}

// Deprecated
QPointF RenderArea::getAlphaBeta(int Mode)
{
    // r = 1160 / 2 = 580;
    // RadiusF = 580 / 1080
if( Mode == 0) {

    double r = (int)sqrt(pow(currPos.x()-Center.x(), 2) + pow(currPos.y()-Center.y(), 2));
    double alpha = r / this->height() * 90 / RadiusF ; // !! estimated
    double angle;
    double delta_x = (currPos.x()-Center.x());
    double delta_y = -(currPos.y()-Center.y());
    if ( currPos.x() == Center.x() )
    angle = 0;
    else
    angle = ( atan2(delta_y,delta_x) * 180 ) / 3.14159265;
    angle = 90 - angle ;
    qDebug() << currPos.x() << " " << currPos.y();
    qDebug() << angle;
    return QPointF(alpha, angle);
}
else {
    double theta_x = -(double)(currPos.y()-Center.y()) / this->height() * 90 / RadiusF ;
    double theta_y = (double)(currPos.x()-Center.x()) / this->height() * 90 / RadiusF ;
    return QPointF(theta_x, theta_y);
}

}


QPointF RenderArea::getRhoHieghtRatioAndBeta(int Mode)
{
    double r = (int)sqrt(pow(currPos.x()-Center.x(), 2) + pow(currPos.y()-Center.y(), 2));
    double RhoHeightRatio = r/this->height();
if( Mode == 0) {
    double angle;
    double delta_x = (currPos.x()-Center.x());
    double delta_y = -(currPos.y()-Center.y());
    if ( delta_x == 0 )
    angle = 0;
    else
    angle = ( atan2(delta_y,delta_x) * 180 ) / 3.14159265;
    angle = 90 - angle ;
    // qDebug() << currPos.x() << " " << currPos.y();
    // qDebug() << angle;
    return QPointF(RhoHeightRatio, angle);
}
else {
    // double theta_x = -(double)(currPos.y()-Center.y()) / this->height() * 90 / RadiusF ;
    // double theta_y = (double)(currPos.x()-Center.x()) / this->height() * 90 / RadiusF ;

    double xHeightRatio = -(double)(currPos.y()-Center.y()) / this->height();
    double yHeightRatio = (double)(currPos.x()-Center.x()) / this->width();
    return QPointF(xHeightRatio, yHeightRatio);
}

}


void RenderArea::setSize(int Size)
{
    currSize = Size;
    update();
}

void RenderArea::setAnypointPoly(QPointF poly[], int n, int TopEdgeStart, int TopEdgeEnd, QPointF polyCenter)
{
    for(int i=0;i< anyPoly_Max;i++){
        anyPoly[i].setX(poly[i].x());
        anyPoly[i].setY(poly[i].y());
    }
    anyPolyLength = n;
    anyPolyTopEdgeStart = TopEdgeStart;
    anyPolyTopEdgeEnd = TopEdgeEnd;
    anyPolyCenter = polyCenter;
    drawType = DrawType::ANYPOINT;
    update();
}

void RenderArea::setPanorama()
{
    drawType = DrawType::PANORAMA;
    update();
}
void RenderArea::updateAnypointPoly()
{
    if ( anyPolyLength > 0 ) {
        for(int i=0;i< anyPolyLength; i++){
                anyPolytoDraw[i].setX(anyPoly[i].x()*this->width());
                anyPolytoDraw[i].setY(anyPoly[i].y()*this->height());
        }
    }
    anyPolyCentertoDraw.setX(anyPolyCenter.x()*this->width());
    anyPolyCentertoDraw.setY(anyPolyCenter.y()*this->height());
}
void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    QPolygon poly;
    QPainterPath pathTopEdge;
    Radius = this->height()*RadiusF ;
    painter.setBrush(brush);
    QPen out_pen(Qt::black, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    QPen top_pen(Qt::red, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    QPen white_pen(Qt::white, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    QPen black_pen(Qt::black, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    QPen green_pen(Qt::green, 2, Qt::PenStyle(1), Qt::PenCapStyle(1), Qt::PenJoinStyle(1));
    int crossHair_Size = 10;
    // QPoint circle(currSize,currSize);
    switch ( drawType ) {
    case DrawType::ANYPOINT :
        updateAnypointPoly();

        if ( anyPolyLength > 0 ) {
            for (int i=0 ;i<anyPolyLength;i++) {
                poly << anyPolytoDraw[i];
            }
            poly << anyPolytoDraw[0];
            painter.setPen(out_pen);
            painter.drawPolygon(poly);

            if( anyPolyTopEdgeStart != anyPolyTopEdgeEnd ) {
                pathTopEdge.moveTo(anyPolytoDraw[anyPolyTopEdgeStart]);
                for (int i=anyPolyTopEdgeStart+1 ;i<=anyPolyTopEdgeEnd;i++) {
                    pathTopEdge.lineTo(anyPolytoDraw[i]);
                }
                painter.setPen(top_pen);
                painter.drawPath(pathTopEdge);
            }

            QPainterPath path;
            path.addPolygon(poly);
            painter.fillPath(path, QBrush(QColor(200, 200, 100, 60)));

        }
        if ( isDrawCenterEnabled ) {
            pathTopEdge = QPainterPath();
            pathTopEdge.moveTo(Center.x()-crossHair_Size+1,Center.y()+1);
            pathTopEdge.lineTo(Center.x()+crossHair_Size+1,Center.y()+1);
            pathTopEdge.moveTo(Center.x()+1,Center.y()-crossHair_Size+1);
            pathTopEdge.lineTo(Center.x()+1,Center.y()+crossHair_Size+1);
            painter.setPen(black_pen);
            painter.drawPath(pathTopEdge);
            pathTopEdge = QPainterPath();
            pathTopEdge.moveTo(Center.x()-crossHair_Size,Center.y());
            pathTopEdge.lineTo(Center.x()+crossHair_Size,Center.y());
            pathTopEdge.moveTo(Center.x(),Center.y()-crossHair_Size);
            pathTopEdge.lineTo(Center.x(),Center.y()+crossHair_Size);
            painter.setPen(white_pen);
            painter.drawPath(pathTopEdge);
            painter.setPen(green_pen);
            painter.drawEllipse(anyPolyCentertoDraw, 10, 10);
        }
        break;
    case DrawType::PANORAMA :
        painter.setPen(green_pen);
        painter.drawEllipse( currPos, 10, 10 );
        break;
    }
    /*
    // painter.drawEllipse(QRect(10,10,120,120));
    // painter.save();
    double angle;
    double delta_x = (currPos.x()-Center.x());
    double delta_y = -(currPos.y()-Center.y());
    if ( currPos.x() ==  Center.x() )
    angle = 0;
    else
    angle = ( atan2(delta_y,delta_x) * 180 ) / 3.14159265;
    angle = 90 - angle;
    //qDebug() << currPos.x() << " " << currPos.y();
    //qDebug() << angle;

    // painter.translate(currPos);
    // painter.rotate(angle);
    //// painter.translate(-currPos);
    // painter.setPen(pen);
    // painter.drawEllipse(QRect(-circle.x()/2, -circle.y()/2, circle.x(), circle.y()));
    // painter.restore();

    //painter.setRenderHint(QPainter::Antialiasing, false);
    //painter.setPen(palette().dark().color    QBrush brush0;
    //painter.setBrush(Qt::NoBrush);
    //painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
*/

}
void RenderArea::mousePressEvent(QMouseEvent *event)
{
    emit pressed(event);
}
void RenderArea::mouseDoubleClickEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton )
    {
    setPos(0, 0);
    }
    // QMainWindow::mouseDoubleClickEvent( e );

    emit doubleclicked(event);
}
void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(event);
}
void RenderArea::wheelEvent(QWheelEvent *event)
{
    emit wheeled(event);
}
void RenderArea::resizeEvent(QResizeEvent* event){

    Center.setX(this->width()*CenterF.x());
    Center.setY(this->height()*CenterF.y());
    Radius = this->height()*RadiusF ;
    setPos(currAlpha, currBeta);
    // update();
}
