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

#include "Painter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>

Painter::Painter()
{
    textFont.setPixelSize(20);
    epiFont.setPixelSize(15);
    textPen = QPen(Qt::black);
    backImage.load(":/images/skorea.png");
    backImage = backImage.scaled(IMAGE_X_WIDTH, IMAGE_Y_HEIGHT, Qt::KeepAspectRatio);
}

void Painter::paint(QPainter *painter, QPaintEvent *event, _BINARY_EEW_PACKET myeewp, _BINARY_QSCD_PACKET myqscd)
{
    QString chanS;
    if(chanID == 0) chanS = "East/West Channel";
    else if(chanID == 1) chanS = "North/South Channel";
    else if(chanID == 2) chanS = "Up/Down Channel";
    else if(chanID == 3) chanS = "Horizontal Channel";
    else if(chanID == 4) chanS = "Total(3D) Channel";

    if(myqscd.numPGAsta > MAX_NUM_STATION)
        return;

    isEvent = false;

    painter->fillRect(event->rect(), backImage);
    painter->save();

    if(myeewp.numEVENT != 0)
    {
        for(int i=0;i<myeewp.numEVENT;i++)
        {
            _event = myeewp.eventlist[i];

            if(myqscd.dataTime >= _event.eventEpochStartTime &&
                    myqscd.dataTime < _event.eventEpochStartTime + EVENT_DURATION)
            {
                isEvent = true;
                break;
            }
        }
    }

    if(isEvent)
    {
        // if maxPGAList is empty then insert staList into maxPGAList
        if(maxPGAList.isEmpty())
        {
            for(int i=0;i<myqscd.numPGAsta;i++)
            {
                _STATION sta = myqscd.staList[i];
                maxPGAList.append(sta);
            }
        }
        else
        {
            for(int i=0;i<myqscd.numPGAsta;i++)
            {
                _STATION sta = myqscd.staList[i];
                bool needInsert = true;
                for(int j=0;j<maxPGAList.size();j++)
                {
                    _STATION maxsta = maxPGAList.at(j);
                    if(sta.index == maxsta.index)
                    {
                        if(sta.lastPGA[0] > maxsta.lastPGA[0]) maxsta.lastPGA[0] = sta.lastPGA[0];
                        if(sta.lastPGA[1] > maxsta.lastPGA[1]) maxsta.lastPGA[1] = sta.lastPGA[1];
                        if(sta.lastPGA[2] > maxsta.lastPGA[2]) maxsta.lastPGA[2] = sta.lastPGA[2];
                        if(sta.lastPGA[3] > maxsta.lastPGA[3]) maxsta.lastPGA[3] = sta.lastPGA[3];
                        if(sta.lastPGA[4] > maxsta.lastPGA[4]) maxsta.lastPGA[4] = sta.lastPGA[4];
                        needInsert = false;
                        maxPGAList.replace(j, maxsta);
                        break;
                    }
                }
                if(needInsert)
                    maxPGAList.append(sta);
            }
        }

        // draw max pga
        for(int i=0;i<maxPGAList.size();i++)
        {
            _STATION sta = maxPGAList.at(i);
            QColor col;
            col.setRgb(redColor(sta.lastPGA[chanID]), greenColor(sta.lastPGA[chanID]), blueColor(sta.lastPGA[chanID]));
            QBrush brush = QBrush(col);
            painter->setBrush(brush);
            painter->drawEllipse(QPoint(sta.mapX, sta.mapY), 5, 5);
        }

        // draw epicenter
        QBrush brush = QBrush(QColor(Qt::red));
        painter->setBrush(brush);
        painter->drawEllipse(QPoint(_event.mapX, _event.mapY), 8, 8);
        painter->setBrush(Qt::white);
        painter->drawRect(QRect(_event.mapX - 20, _event.mapY + 12, 40, 15));
        painter->setPen(textPen);
        painter->setFont(epiFont);
        painter->drawText(QRect(_event.mapX - 25, _event.mapY + 5, 50, 30), Qt::AlignCenter, "M" + QString::number(_event.mag, 'f', 1));

        int eqFlowTimeSec = myqscd.dataTime - _event.eventEpochStartTime;

        qreal radiusP = eqFlowTimeSec * P_VEL;
        qreal radiusS = eqFlowTimeSec * S_VEL;
        painter->setPen(Qt::blue);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(_event.mapX, _event.mapY), radiusP, radiusP);
        painter->setPen(Qt::red);
        painter->drawEllipse(QPointF(_event.mapX, _event.mapY), radiusS, radiusS);

        painter->setPen(textPen);
        painter->setFont(textFont);

        QDateTime et;
        et.setTimeSpec(Qt::UTC);
        et.setTime_t(_event.eventEpochStartTime);

        painter->drawText(QRect(500, 85, 300, 20), Qt::AlignRight, "EEW ID:" + QString::number(_event.evid));
        painter->drawText(QRect(500, 105, 300, 20), Qt::AlignRight, et.toString("yyyy-MM-dd hh:mm:ss") + " (UTC)");
        painter->drawText(QRect(500, 125, 300, 20), Qt::AlignRight, "M" + QString::number(_event.mag, 'f', 1));

    }
    else
    {
        maxPGAList.clear();

        for(int i=0;i<myqscd.numPGAsta;i++)
        {
            _STATION sta = myqscd.staList[i];
            QColor col;
            col.setRgb(redColor(sta.lastPGA[chanID]), greenColor(sta.lastPGA[chanID]), blueColor(sta.lastPGA[chanID]));
            QBrush brush = QBrush(col);
            painter->setBrush(brush);
            painter->drawEllipse(QPoint(sta.mapX, sta.mapY), 5, 5);
        }
    }

    painter->restore();
    painter->setPen(textPen);
    painter->setFont(textFont);
    painter->drawText(QRect(500, 5, 300, 20), Qt::AlignRight, dataSrc.section(",", 0, 0));
    painter->drawText(QRect(500, 25, 300, 20), Qt::AlignRight, dataSrc.section(",", 1, 1));
    painter->drawText(QRect(500, 45, 300, 20), Qt::AlignRight, chanS);
}

int Painter::redColor(float gal)
{
    int color ;

    // red color value
    if( gal <= 0.0098 )
    {
      color = 191 ;
    }
    else if (gal > 0.0098 && gal <= 0.0392)
    {
      color = gal * (-3265.31) + 223 ;
    }
    else if (gal > 0.0392 && gal <= 0.0784)
    {
      color = 95 ;
    }
    else if (gal > 0.0784 && gal <= 0.098)
    {
      color = gal * 3265.31 - 161 ;
    }
    else if (gal > 0.098 && gal <= 0.98)
    {
      color = gal * 103.82 + 148.497 ;
    }
    else if (gal > 0.98 && gal <= 147)
    {
      color = 255 ;
    }
    else if (gal > 147 && gal <= 245)
    {
      color = -0.00333195 * pow(gal,2) + 0.816327 * gal + 207 ;
    }
    else if (gal > 245)
      color = 207 ;

    return color ;
}

int Painter::greenColor(float gal)
{
    int color ;
    // red color value
    if( gal <= 0.98 )
    {
      color = 255 ;
    }
    else if (gal > 0.98 && gal <= 9.8)
    {
      color = -0.75726 * gal * gal - 0.627943 * gal + 255.448 ;
    }
    else if (gal > 0.98 && gal <= 245)
    {
      color = 0.00432696 * gal * gal - 1.84309 * gal + 192.784 ;
      if(color < 0)
        color = 0 ;
    }
    else if (gal > 245)
      color = 0 ;

    return color ;
}

int Painter::blueColor(float gal)
{
    int color ;

    // red color value
    if( gal <= 0.0098 )
    {
      color = 255 ;
    }
    else if (gal > 0.0098 && gal <= 0.098)
    {
      color = -19799.2 * gal * gal + 538.854 * gal + 260.429 ;
    }
    else if (gal > 0.098 && gal <= 0.98)
    {
      color = -35.4966 * gal * gal - 65.8163 * gal + 116.264 ;
    }
    else if (gal > 0.98 && gal <= 3.92)
    {
      color = -5.10204 * gal + 20 ;
    }
    else if (gal > 3.92)
    {
      color = 0 ;
    }

    if(color > 255)
      color = 255 ;

    return color ;
}
