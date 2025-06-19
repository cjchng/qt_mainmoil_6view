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

#ifndef RENDERAREA_H
#define RENDERAREA_H
#include <math.h>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QPolygon>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);
    QPointF CenterF;   // (0..1,0..1)
    QPoint Center;
    float RadiusF;
    int Radius;
    int currAlpha, currBeta;
    QPoint currPos;
    int currSize;
    void setCenter(QPointF center, float r);
    void setSize(int Size);
    void setAnypointPoly(QPointF poly[], int n, int TopEdgeStart, int TopEdgeEnd, QPointF polyCenter);
    void setPanorama();
    QPointF getAlphaBeta(int Mode);
    QPointF getRhoHieghtRatioAndBeta(int Mode);
    bool isDrawCenterEnabled = false;

public slots:
    void setPos(QPoint pos);
    void setPos(int alpha, int beta);
    QPointF getPosF();
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent ( QWheelEvent * event );
private:
    static const int anyPoly_Max = 40 ;
    QPointF anyPoly[anyPoly_Max];
    QPoint anyPolytoDraw[anyPoly_Max];
    QPointF anyPolyCenter;
    QPoint anyPolyCentertoDraw;
    int anyPolyLength = 0;
    int anyPolyTopEdgeStart = 0, anyPolyTopEdgeEnd = 0;
    enum class DrawType {ANYPOINT, PANORAMA };
    DrawType drawType = DrawType::ANYPOINT;
    QPen pen;
    QBrush brush;
    QPixmap pixmap;
    void resizeEvent(QResizeEvent* event);
    void updateAnypointPoly();
signals:
void pressed(QMouseEvent *event);
void clicked(QMouseEvent *event);
void doubleclicked(QMouseEvent *event);
void wheeled( QWheelEvent *event );
};


#endif // RENDERAREA_H
