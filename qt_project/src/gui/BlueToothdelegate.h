/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifidelegate.h
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
#ifndef BLUETOOTHDELEGATE_H
#define BLUETOOTHDELEGATE_H

#include <QItemDelegate>

#define BT_IMAGE_RENAME_WIDTH 22
#define BT_IMAGE_RENAME_HEIGHT 20
#define BT_IMAGE_CONNECT_WIDTH 14
#define BT_IMAGE_CONNECT_HEIGHT 22

#define WIFI_IMAGE_DISTANCE 36

class BlueToothDelegate : public QItemDelegate //QStyledItemDelegate
{
    Q_OBJECT
public:
    BlueToothDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    virtual void drawFocus(QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect) const
    {
        Q_UNUSED(painter)
        Q_UNUSED(option)
        Q_UNUSED(rect)
    }

    virtual void drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
        const QRect &rect, Qt::CheckState state) const
    {
        Q_UNUSED(painter)
        Q_UNUSED(option)
        Q_UNUSED(rect)
        Q_UNUSED(state)
    }
private:
    //   0   —— (-55)dbm  满格(4格)信号
    // (-55) —— (-70)dbm  3格信号
    // (-70) —— (-85)dbm　2格信号
    // (-85) —— (-100)dbm 1格信号
    int calculateSignalLevel(int rssi, int numLevels) const;    //根据信号level计算，强度等级
};

#endif // BLUETOOTHDELEGATE_H
/*********************************************************************************************************
** End of BlueToothdelegate.h
*********************************************************************************************************/