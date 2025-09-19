/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifimodel.cpp
** Latest Version:             V1.0.1
** Latest modified Date:       2023/03/02
** Modified by:                Wang Jinge
** Descriptions:               添加注释信息
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth列表模型类
**
*********************************************************************************************************/
#include "BlueToothmodel.h"
#include <QAbstractItemModel>
#include <QColor>

BlueToothModel::BlueToothModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_blueToothState(BlueTooth_DISCONNECT)
{
#ifdef _WIN32
    m_HotspotList.clear();
    HOTSPOTINFO_SINGLE_BLUETOOTH hotspot1;
    HOTSPOTINFO_SINGLE_BLUETOOTH hotspot2;
    HOTSPOTINFO_SINGLE_BLUETOOTH hotspot3;
    hotspot1.m_ssid = QString("545451");
    hotspot1.m_status = true;
    hotspot1.m_orderLevel = 1;
    hotspot2.m_ssid = QString("we");
    hotspot2.m_status = false;
    hotspot2.m_orderLevel = 2;
    hotspot3.m_ssid = QString("best_otdt");
    hotspot3.m_status = false;
    hotspot3.m_orderLevel = 3;
    m_HotspotList.append(hotspot1);
    m_HotspotList.append(hotspot2);
    m_HotspotList.append(hotspot3);
#endif
}

int BlueToothModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

int BlueToothModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

Qt::ItemFlags BlueToothModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        f |= Qt::ItemNeverHasChildren;
    }
    return f;
}

QVariant BlueToothModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int num = index.row();// + index.column() * ROW_COUNT;

    if (num >= m_HotspotList.count())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        HOTSPOTINFO_SINGLE_BLUETOOTH ret = m_HotspotList.at(num);
        return QVariant::fromValue(ret);
    }

    if (role == Qt::ForegroundRole)
    {
        QColor color;
        if ((m_blueToothState == BlueTooth_CONNECT) && (m_HotspotList.at(num).m_bssid == m_connectedHotspot.m_bssid))
        {
            //红色
            color.setRgb(255,0,0);
            return QVariant::fromValue(color);
        }
        else
        {
            //黑色
            color.setRgb(0, 0, 0);
            return QVariant::fromValue(color);
        }
    }

    return QVariant();
}
/**
 * @brief 返回指模型中指定父项以及行列的项的索引
 * @param row
 * @param column
 * @param parent
 * @return  QModelIndex 指定项索引
 */
QModelIndex BlueToothModel::index(int row, int column, const QModelIndex &parent) const
{
    return hasIndex(row, column, parent) ?
                createIndex(row, column) : QModelIndex();
}
/**
 * @brief 返回表头数据
 * @param section
 * @param orientation
 * @param role
 * @return

QVariant wifimodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if ((role == Qt::DisplayRole) && (section < columnCount())) {
            switch(section)
            {
            case 0:
                return QString("ssid");
                break;
            case 1:
                return QString("signal level");
                break;
            case 2:
                return QString("encrypt");
                break;
            default:
                break;

            }
        }
    }

    return QVariant();
}*/
/**
 * @brief 返回指定索引所属的父索引
 * @param child
 * @return
 */
QModelIndex BlueToothModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}


void BlueToothModel::refreshHotspotList(QList<HOTSPOTINFO_SINGLE_BLUETOOTH> hotspotList)
{
    beginRemoveRows(QModelIndex(), 0, ROW_COUNT*2);
    m_HotspotList.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, ROW_COUNT);
    m_HotspotList = hotspotList;
    endInsertRows();
    beginInsertRows(QModelIndex(), ROW_COUNT, ROW_COUNT*2);
    m_HotspotList = hotspotList;
    endInsertRows();
}

/**
* @brief 设置已连接的热点
* @param
* @return
*/
void BlueToothModel::setConnectedHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH hotspot)
{
    m_connectedHotspot = hotspot;
}

/**
* @brief 设置状态
* @param hotspotList
* @return
*/
void BlueToothModel::setBlueToothStatus(BlueTooth_STATUS status)
{
    m_blueToothState = status;
}


bool BlueToothModel::hotspotInfo(int index, HOTSPOTINFO_SINGLE_BLUETOOTH &hotspotInfo)
{
    if (index < m_HotspotList.count())
    {
        hotspotInfo = m_HotspotList.at(index);
        return true;
    }
    return false;
}

void BlueToothModel::refresh(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    //为了消除item选中状态，使离开屏幕后消除黄色选中条
    beginRemoveRows(QModelIndex(), 0, ROW_COUNT);
    endRemoveRows();
}
/*********************************************************************************************************
** End of BlueToothmodel.cpp
*********************************************************************************************************/