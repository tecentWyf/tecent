/****************************************Copyright (c)****************************************************
**
**                                       Giettek InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Wifi.cpp
** Latest Version:             V1.0.3
** Latest modified Date:       2023/06/01
** Modified by:                Wang Jinge
** Descriptions:               暂时取消控制WiFi电源
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Wang Jinge
** Created date:               2022/11/10
** Descriptions:               BlueTooth业务类
**
*********************************************************************************************************/
#include "BlueTooth.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QHeaderView>
#include "BlueToothdelegate.h"
#include "MsgBoxFrame.h"
#include "gui/FileWidgets.h"
#include "HintMsgBox.h"
BlueTooth::BlueTooth(QObject *parent)
    : QObject(parent)
    , m_enable(false)
    , m_enableState(false)
    , m_stateInfo("")
    , m_IPAddress("")
    , m_blueToothState(BlueTooth_DISCONNECT)
    , m_blueToothError(BlueTooth_NONE)
    , m_blueTooth(NULL)
{
    emit ConnectedHotspotChanged();
    //m_blueTooth = new BlueToothplugin();
    setModel(new BlueToothModel());
    connect(this, &BlueTooth::objectNameChanged, this, &BlueTooth::onObjectNameChanged);
}

BlueTooth::~BlueTooth()
{

}

void BlueTooth::setBlueToothPlugin(QObject *blueToothPlugin)
{
    if (blueToothPlugin)
    {
        m_blueTooth = qobject_cast<BlueToothPluginInterface*>(blueToothPlugin);

        connect(m_blueTooth, &BlueToothPluginInterface::refresh,
            this, &BlueTooth::update);
        connect(m_blueTooth, &BlueToothPluginInterface::state,
            this, &BlueTooth::bluetoothStateChanged);
        connect(m_blueTooth, &BlueToothPluginInterface::error,
            this, &BlueTooth::bluetoothError);
        connect(m_blueTooth, &BlueToothPluginInterface::scanning,
            this, &BlueTooth::scanState);
        connect(m_blueTooth, &BlueToothPluginInterface::connectedHotspotChange,
            this, &BlueTooth::setConnectedHotspot);

        if (enableState() != isEnabled())
        {
            //setEnabled(enableState());
        }
    }
}


void BlueTooth::update(void)
{
    QList<HOTSPOTINFO_SINGLE_BLUETOOTH> List;
    List = m_blueTooth->BlueToothList();
    //qDebug()<<List.count();

    model()->refreshHotspotList(List);
    emit refresh();
}

void BlueTooth::selectNetwork(QModelIndex modelIndex)
{
    m_model->refresh(modelIndex, modelIndex);
    emit refresh();
    if (m_model->hotspotInfo(modelIndex.row()// + modelIndex.column() * ROW_COUNT
        , m_selectHotspot))
    {
        if (0 == modelIndex.column())
        {
            if (m_selectHotspot.m_status)
            {
                m_blueTooth->breakConnecting();
            }
            else
            {
                if (!connectedHotspot().m_status)
                    m_blueTooth->connectHotspot(m_selectHotspot);
                //setConnectedHotspot(m_selectHotspot);
            }
        }
        else if (1 == modelIndex.column())
        {
            if (3 == m_selectHotspot.m_orderLevel)
            {
                return;
            }
            MsgBoxFrame::StdButton button = HintMsgBox::message(
                tr("Ignore"), tr("Ignore this device?"), QPixmap("resources/msgBox/warn.png"), MsgBoxFrame::Ok | MsgBoxFrame::Cancel,
                MsgBoxFrame::Center, MainWindow::instance()
                );
            if (MsgBoxFrame::Cancel == button)
            {
                return;
            }

          //   if(m_selectHotspot.m_status)
          //  {
                qDebug() << "\nbreakConnecting ===========================";
                m_blueTooth->breakConnecting();
                
         //   }

            m_blueTooth->ignoreHotspot(m_selectHotspot);
        }
        else if (2 == modelIndex.column())
        {
            if (m_selectHotspot.m_status)
            {
                RenameDialog d;
                d.setTextLabel(tr("Rename"));
                MsgBoxFrame::StdButton button = MsgBoxFrame::commonMsgBox(&d,
                    MsgBoxFrame::Center,
                    MsgBoxFrame::Ok | MsgBoxFrame::Cancel,
                    MainWindow::instance());

                if (MsgBoxFrame::Cancel == button)
                {
                    return;
                }

                QString dirName = d.getText();
                if (!dirName.isEmpty())
                {
                    m_blueTooth->renameHotspot(dirName);
                }
                else if (dirName.isEmpty())
                {
                    /*MsgBoxFrame::StdButton button = */HintMsgBox::message(
                        tr("Rename"), tr("Name can not be empty!"), QPixmap("resources/msgBox/warn.png"), MsgBoxFrame::Ok,
                        MsgBoxFrame::Center, MainWindow::instance()
                        );
                }
            }
        }
    }
}

// 切换语言之后，wifistateinfo没有及时翻译，进入wifi设置界面手动更新一次
void BlueTooth::updateStateInfo()
{
#ifndef _WIN32
    //if (BlueTooth_NONE != m_wifiError)
    //{
    //    emit m_blueTooth->error(m_wifiError);
    //    return;
    //}
    emit m_blueTooth->state(m_blueToothState);
#endif
    ////打开wifi电源
    //openPower();
}

void BlueTooth::openPower()
{
    //打开wifi电源
    QString command = "/etc/wifi_init.sh insmod &";
    qDebug() << "\nExecute command: " << command << "...";
    const int result = system(command.toLatin1().data());
    qDebug() << "Execute command: " << command << " - Result: " << result;
}

void BlueTooth::closePower()
{
    //if (!m_enable)
    //{
    //    //关闭电源
    //    QString command = "/etc/wifi_init.sh rmmod &";
    //    qDebug() << "\nExecute command: " << command << "...";
    //    const int result = system(command.toLatin1().data());
    //    qDebug() << "Execute command: " << command << " - Result: " << result;
    //}
}

void BlueTooth::onObjectNameChanged(const QString& name)
{
    Q_UNUSED(name)
        DECLARE_SETTINGS(this);
    ////开机检测是否需要打开电源
    //if (m_enableState)
    //{
    //    openPower();
    //}
}

void BlueTooth::bluetoothStateChanged(BlueTooth_STATUS state)
{
    model()->setBlueToothStatus(state);
    model()->setConnectedHotspot(m_blueTooth->getConnectedHotspot());
    //if (m_blueToothState != BlueTooth_CONNECT && state == BlueTooth_CONNECT)
    //{
    //    //m_IPAddress = QString("IP: ") + m_blueTooth->IPAddress();
    //    emit BlueToothIPAddressChanged();
    //    //model()->setConnectedHotspot(m_blueTooth->getConnectedHotspot());
    //}
    //else if (m_blueToothState != BlueTooth_DISCONNECT && state == BlueTooth_DISCONNECT)
    //{
    //    m_IPAddress.clear();
    //    emit BlueToothIPAddressChanged();
    //}

    m_blueToothState = state;
    switch (m_blueToothState)
    {
    case BlueTooth_DISCONNECT:
        qDebug() << "\nBlueTooth_DISCONNECT====================================== ";
        setStateInfo(tr("Disconnect"));
        setConnectedHotspot(m_blueTooth->getConnectedHotspot());
        //emit bluetoothIntensionChanged(-1);
        break;
    case BlueTooth_CONNECTING:
        qDebug() << "\n Connect... ====================================== ";
        setStateInfo(tr("Connect..."));
        break;
    case BlueTooth_CONNECT:
        setStateInfo(tr("Connected"));
        setConnectedHotspot(m_blueTooth->getConnectedHotspot());
        //emit bluetoothIntensionChanged(m_blueTooth->getConnectedHotspot().m_signallevel);
        break; 
    case BlueTooth_CONNECTFAIL:
        setStateInfo(tr("Connect fail"));
        break;
    case BlueTooth_IGNORE:
       qDebug() << "\nBlueTooth_IGNORE====================================== ";
       
       HintMsgBox::message(
            tr("Ignore"), tr("Successfully removed this device!"), QPixmap("resources/msgBox/warn.png"), MsgBoxFrame::Ok,
            MsgBoxFrame::Center, MainWindow::instance()
            );
        break;
    default:
        break;
    }
}

void BlueTooth::bluetoothError(BlueTooth_ERROR error)
{
    m_blueToothError = error;
    switch (m_blueToothError)
    {
    case BlueTooth_NONE:
        break;
    case BlueTooth_AUTHENTICATIONFAIL:
        setStateInfo(tr("Disconnect"));
        break;
    case BlueTooth_DHCPFAIL:
        setStateInfo(tr("Disconnect"));
        break;
    case BlueTooth_UNKNOWNERROR:
        setStateInfo(tr("Unknown error"));
        break;
    default:
        break;
    }
}

void BlueTooth::scanState(bool state)
{
    if (state && m_blueToothState != BlueTooth_CONNECT)
    {
        setStateInfo(tr("Is scanning"));
    }
}
/*********************************************************************************************************
** End of BlueTooth.cpp
*********************************************************************************************************/