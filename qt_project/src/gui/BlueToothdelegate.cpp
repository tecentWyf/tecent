/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifidelegate.cpp
** Latest Version:             V1.0.1
** Latest modified Date:       2023/03/02
** Modified by:                Wang Jinge
** Descriptions:               添加注释信息
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth列表代理类
**
*********************************************************************************************************/
#include "BlueToothdelegate.h"
#include "blueToothplugininterface.h"
#include <QIcon>
#include <QPainter>
#include <QDebug>
#include <QColor>

#define MIN_RSSI -100
#define MAX_RSSI -55
BlueToothDelegate::BlueToothDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

void BlueToothDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if(!index.data().isNull())
    {
        QItemDelegate::paint(painter, option, index);

        QRect tableRect = option.rect;

        if (index.column() == 2)
        {
            bool isSelected = (option.state & QStyle::State_Selected);

            painter->drawPixmap(tableRect.x() + (tableRect.width() - BT_IMAGE_RENAME_WIDTH) / 2, tableRect.y() + (tableRect.height() - BT_IMAGE_RENAME_HEIGHT) / 2,
                QPixmap("./skins/skin1/resources/blueToothSettings/blueToothReName.png").
                scaled(BT_IMAGE_RENAME_WIDTH, BT_IMAGE_RENAME_HEIGHT)
                );
            return;
        }

        HOTSPOTINFO_SINGLE_BLUETOOTH hotspotInfo = index.data().value<HOTSPOTINFO_SINGLE_BLUETOOTH>();
        if (index.column() == 1)
        {
            if (3 != hotspotInfo.m_orderLevel)
            {
                painter->drawPixmap(tableRect.x() + (tableRect.width() - BT_IMAGE_RENAME_WIDTH) / 2, tableRect.y() + (tableRect.height() - BT_IMAGE_RENAME_HEIGHT) / 2,
                    QPixmap("./skins/skin1/resources/blueToothSettings/blueToothIgnore.png").
                    scaled(BT_IMAGE_RENAME_WIDTH, BT_IMAGE_RENAME_HEIGHT)
                    );
            }
            return;
        }
        QString ssid = hotspotInfo.m_ssid;
        QRect textRect(tableRect.topLeft() + QPoint(WIFI_IMAGE_DISTANCE,0),
                       tableRect.bottomRight() - QPoint(BT_IMAGE_RENAME_WIDTH+
                                                        BT_IMAGE_CONNECT_WIDTH+
                                                        3*WIFI_IMAGE_DISTANCE,0));

        int xEncrypt = textRect.topRight().x() + WIFI_IMAGE_DISTANCE;
        int yEncrypt = textRect.topRight().y() + (textRect.height() - BT_IMAGE_RENAME_HEIGHT) / 2;

        int xSignal = textRect.topRight().x() + WIFI_IMAGE_DISTANCE + BT_IMAGE_RENAME_WIDTH + 15;
        int ySignal = textRect.topRight().y() + (textRect.height() - BT_IMAGE_CONNECT_HEIGHT) / 2;

        QPixmap m_signalIcon;

        //painter->drawPixmap(tableRect,QPixmap("./skins/skin1/resources/wifiSettings/wifiSelectedBg.png"));
        QColor color = index.model()->data(index, Qt::ForegroundRole).value<QColor>();
        painter->setPen(color);
        painter->drawText(textRect, Qt::AlignVCenter, ssid);
        qDebug() << "hotspotInfo.m_ssid =" << hotspotInfo.m_ssid << "hotspotInfo.m_status = " << hotspotInfo.m_status;
        if(hotspotInfo.m_status)
        {
            painter->drawPixmap(xSignal, ySignal,
                                QPixmap("./skins/skin1/resources/blueToothSettings/blueToothConnect.png").
                                scaled(BT_IMAGE_CONNECT_WIDTH, BT_IMAGE_CONNECT_HEIGHT)
                                );
        }
        else
        {
            painter->drawPixmap(xSignal, ySignal,
                QPixmap("./skins/skin1/resources/blueToothSettings/blueToothNotConnect.png").
                scaled(BT_IMAGE_CONNECT_WIDTH, BT_IMAGE_CONNECT_HEIGHT)
                );
        }
    }
    else
    {
        
    }
    
}

int BlueToothDelegate::calculateSignalLevel(int rssi, int numLevels) const
{
    // in general, numLevels is 4
    if (rssi <= MIN_RSSI)
    {
        return 0;
    }
    else if (rssi >= MAX_RSSI)
    {
        return numLevels;
    }
    else
    {
        float inputRange = (MAX_RSSI - MIN_RSSI);
        float outputRange = numLevels;

        return (int)((float)(rssi - MIN_RSSI) * outputRange / inputRange);
    }
}
/*********************************************************************************************************
** End of BlueToothdelegate.cpp
*********************************************************************************************************/