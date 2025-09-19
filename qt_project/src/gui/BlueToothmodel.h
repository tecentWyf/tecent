/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifimodel.h
** Latest Version:             V1.0.2
** Latest modified Date:       2023/03/08
** Modified by:                Wang Jinge
** Descriptions:               修改为1列信息显示
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth列表模型类
**
*********************************************************************************************************/
#ifndef BLUETOOTHMODEL_H
#define BLUETOOTHMODEL_H
#include <QAbstractItemModel>
#include "blueToothplugininterface.h"

#define ROW_COUNT 30
#define COLUMN_COUNT 3

class BlueToothModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BlueToothModel(QObject *parent = 0);

    /**
     * @brief 返回指定父项所包含数据的的行数
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief 返回指定父项所包含数据的的列数
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief 返回指定项的标志
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * @brief 根据指定的索引和角色返回相应的数据
     * @param index
     * @param role
     * @return  QVariant  数据值
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * @brief 返回指模型中指定父项以及行列的项的索引
     * @param row
     * @param column
     * @param parent
     * @return  QModelIndex 指定项索引
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    /**
     * @brief 返回表头数据
     * @param section
     * @param orientation
     * @param role
     * @return

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;*/
    /**
     * @brief 返回指定索引所属的父索引
     * @param child
     * @return
     */
    QModelIndex parent(const QModelIndex &child) const;

    /**
    * @brief 刷新热点列表
    * @param hotspotList
    * @return
    */
    void refreshHotspotList(QList<HOTSPOTINFO_SINGLE_BLUETOOTH> hotspotList);

    /**
    * @brief 设置已连接的热点
    * @param 
    * @return
    */
    void setConnectedHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH);

    /**
    * @brief 设置状态
    * @param hotspotList
    * @return
    */
    void setBlueToothStatus(BlueTooth_STATUS);

    /**
    * @brief 得到索引对应的热点
    * @param index
             hotspotInfo   
    * @return 成功返回true 失败返回false
    */
    bool hotspotInfo(int index, HOTSPOTINFO_SINGLE_BLUETOOTH &hotspotInfo);

    /**
    * @brief 刷新
    * @param 
    * @return 
    */
    void refresh(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
    QList<HOTSPOTINFO_SINGLE_BLUETOOTH>   m_HotspotList;
    HOTSPOTINFO_SINGLE_BLUETOOTH          m_connectedHotspot;                  //<!已连接的热点
    BlueTooth_STATUS            m_blueToothState;                   //<!wifi状态
};

#endif // BLUETOOTHMODEL_H
/*********************************************************************************************************
** End of BLUETOOTHMODEL.h
*********************************************************************************************************/