/****************************************Copyright (c)****************************************************
**
** Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name: BlueToothplugininterface.h
** Latest Version: V1.0.0
** Latest modified Date: 2022/11/10
** Modified by:WangJinge
** Descriptions:bluetooth插件接口类
**
**--------------------------------------------------------------------------------------------------------
** Created by: WangJinge
** Created date: 2022/11/10
** Descriptions:bluetooth插件接口类
**
*********************************************************************************************************/
#ifndef BLUETOOTHPLUGININTERFACE_H
#define BLUETOOTHPLUGININTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QString>
#include <QList>


typedef struct HOTSPOTINFO_SINGLE_BLUETOOTH
{
    QString m_property;                     //device[del]或device[new] 查看设备增删属性
    QString m_ssid;                         //ap名称,最大长度32字节
    QString m_bssid;                        //蓝牙mac地址6c:59:40:04:dc:64
    bool m_status = false;                          //蓝牙状态
    int  m_orderLevel;
}HOTSPOTINFO_SINGLE_BLUETOOTH;
Q_DECLARE_METATYPE(HOTSPOTINFO_SINGLE_BLUETOOTH)

typedef enum BlueTooth_STATUS                     //当前BlueTooth模块状态
{
    BlueTooth_DISCONNECT = 1,
    BlueTooth_CONNECTING,
    BlueTooth_CONNECT,
    BlueTooth_CONNECTFAIL,
    BlueTooth_IGNORE,
}BlueTooth_STATUS;

typedef enum BlueTooth_ERROR                     //BlueTooth模块错误信息
{
    BlueTooth_NONE = 0,
    BlueTooth_OPENFAIL,
    BlueTooth_AUTHENTICATIONFAIL,
    BlueTooth_DHCPFAIL,
    BlueTooth_UNKNOWNERROR
}BlueTooth_ERROR;


class BlueToothPluginInterface : public QObject
{
    Q_OBJECT
public:
    BlueToothPluginInterface(){}
    ~BlueToothPluginInterface(){}

    virtual bool open(void) = 0;
    virtual bool close(void) = 0;
    virtual QList<HOTSPOTINFO_SINGLE_BLUETOOTH> BlueToothList(void) = 0;
    virtual bool connectHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot) = 0;
    virtual HOTSPOTINFO_SINGLE_BLUETOOTH getConnectedHotspot(void) = 0;

    public slots:
    virtual void disconnectHotspot(void) = 0;
    virtual void breakConnecting(void) = 0;
    virtual void refreshList(void) = 0;
    virtual void renameHotspot(QString name) = 0;
    virtual void ignoreHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH) = 0;
signals:
    void refresh(void);
    void scanning(bool);
    void state(BlueTooth_STATUS);
    void error(BlueTooth_ERROR);
    void connectedHotspotChange(HOTSPOTINFO_SINGLE_BLUETOOTH);
};


QT_BEGIN_NAMESPACE

#define BlueToothPluginInterface_iid "com.inno.common.Module"

Q_DECLARE_INTERFACE(BlueToothPluginInterface, BlueToothPluginInterface_iid)
QT_END_NAMESPACE

#endif // BLUETOOTHPLUGININTERFACE_H
/*********************************************************************************************************
** End of BlueToothPluginInterface.h
*********************************************************************************************************/