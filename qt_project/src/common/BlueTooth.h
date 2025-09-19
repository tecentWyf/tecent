/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifi.h
** Latest Version:             V1.0.1
** Latest modified Date:       2023/03/02
** Modified by:                Wang Jinge
** Descriptions:               添加注释信息
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth业务类
**
*********************************************************************************************************/
#ifndef BLUEBOOTH_H
#define BLUEBOOTH_H

#include <QWidget>
#include "blueToothplugininterface.h"
#include "BlueToothmodel.h"
#include <QTableView>
#include "Wifidialog.h"
#include "Application.h"
#include "gui/F21xMainWindow.h"

class BlueTooth : public QObject
{
    Q_OBJECT
        Q_PROPERTY(bool enabled    WRITE setEnabled    READ isEnabled  NOTIFY BlueToothEnableChanged     STORED false)
        Q_PROPERTY(bool enableState    WRITE setEnableState    READ enableState  NOTIFY enableStateChanged)
        Q_PROPERTY(QString stateInfo    READ stateInfo    NOTIFY BlueToothStateInfoChanged   STORED false)
        Q_PROPERTY(QString IPAddress    READ IPAddress    NOTIFY BlueToothIPAddressChanged   STORED false)
        Q_PROPERTY(BlueToothModel *model    READ model      NOTIFY BlueToothModelChanged   STORED false)
        Q_PROPERTY(QObject *blueToothPlugin1 READ blueToothPlugin   WRITE setBlueToothPlugin   STORED false)
        Q_PROPERTY(HOTSPOTINFO_SINGLE_BLUETOOTH connectedHotspot READ connectedHotspot   WRITE setConnectedHotspot   NOTIFY ConnectedHotspotChanged STORED false)
public:
    BlueTooth(QObject *parent = 0);
    ~BlueTooth();

    void setBlueToothPlugin(QObject *blueToothPlugin);
    QObject *blueToothPlugin()  const
    {
        return m_blueTooth;
    }

    void setEnabled(bool enable)
    {
        if (!m_blueTooth)
        {
            F21xMainWindow::showMessage(tr("The bluetooth plugin was not loaded successfully"));
            return;
        }

        m_enable = m_enable ^ enable;
        setEnableState(m_enable);
        if (m_enable)
        {
            m_blueTooth->open();
        }
        else
        {
            setStateInfo("");
            m_blueTooth->close();
        }
        emit BlueToothEnableChanged();
    }
    bool isEnabled(void)
    {
        return m_enable;
    }
    Q_SIGNAL void BlueToothEnableChanged();

    void setEnableState(bool enable)
    {
        if (m_blueTooth && enable != isEnabled())
        {
            setEnabled(enable);
        }

        if (m_enableState != enable)
        {
            m_enableState = enable;
            emit enableStateChanged();
        }
    }
    bool enableState(void)
    {
        return m_enableState;
    }
    Q_SIGNAL void enableStateChanged();

    void setStateInfo(QString info)
    {
        if (m_stateInfo != info)
        {
            m_stateInfo = info;
            emit BlueToothStateInfoChanged();
        }
    }
    QString stateInfo(void)
    {
        return m_stateInfo;
    }
    Q_SIGNAL void BlueToothStateInfoChanged();

    QString IPAddress(void)
    {
        return m_IPAddress;
    }
    Q_SIGNAL void BlueToothIPAddressChanged();

    void setModel(BlueToothModel *BlueToothModel)
    {
        if (BlueToothModel)
        {
            m_model = BlueToothModel;
            emit BlueToothModelChanged();
        }
    }
    BlueToothModel * model() const
    {
        return m_model;
    }
    Q_SIGNAL void BlueToothModelChanged();


    HOTSPOTINFO_SINGLE_BLUETOOTH connectedHotspot()
    {
        return m_connectedHotspot;
    }
    void setConnectedHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH connectedHotspot)
    {
        m_connectedHotspot = connectedHotspot;
        update();
        emit ConnectedHotspotChanged();
    }
    Q_SIGNAL void ConnectedHotspotChanged();

    Q_SIGNAL void bluetoothIntensionChanged(int bluetoothIntension);
    Q_SIGNAL void refresh();
public slots:
    void update(void);
    void bluetoothStateChanged(BlueTooth_STATUS);
    void bluetoothError(BlueTooth_ERROR);
	void scanState(bool);

    void selectNetwork(QModelIndex);
    void updateStateInfo();
    void openPower();
    void closePower();

    void onObjectNameChanged(const QString& name);

private:
    bool m_enable;
    bool m_enableState;
    QString m_stateInfo;
    QString m_IPAddress;
    BlueToothModel *m_model;

    HOTSPOTINFO_SINGLE_BLUETOOTH m_selectHotspot;
    HOTSPOTINFO_SINGLE_BLUETOOTH m_connectedHotspot;
    BlueTooth_STATUS m_blueToothState;
    BlueTooth_ERROR m_blueToothError;

    BlueToothPluginInterface *m_blueTooth;
};

#endif // BLUEBOOTH_H
/*********************************************************************************************************
** End of BlueTooth.h
*********************************************************************************************************/