#include "blueToothPlugin.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <iconv.h>
#endif
static bool terminateProcess = false;

BlueToothplugin::BlueToothplugin()
    : m_enable(false)
{
    qRegisterMetaType<HOTSPOTINFO_SINGLE_BLUETOOTH>("HOTSPOTINFO_SINGLE_BLUETOOTH");
    qRegisterMetaType<BlueTooth_STATUS>("BlueTooth_STATUS");
    qRegisterMetaType<BlueTooth_ERROR>("BlueTooth_ERROR");

    m_hotspotList.clear();
    m_Timer = new QTimer;
    m_Timer->setInterval(10000);

    m_service = new BlueToothService;
    m_Thread = new QThread;
    m_scanProcess = new QProcess;
    connect(m_Timer,&QTimer::timeout,this,&BlueToothplugin::scanTimeout);
    connect(this,&BlueToothplugin::scanHotspot,m_service,&BlueToothService::scan);
    connect(this,&BlueToothplugin::connectNet,
            m_service,&BlueToothService::connectNetwork);
    connect(m_service,&BlueToothService::scanResult,
            this,&BlueToothplugin::receiveHotspotList);
    connect(m_service,&BlueToothService::connectState,
            this,&BlueToothplugin::receiveConnectState);
    connect(m_service,&BlueToothService::connectedHotspotChange,
            this,&BlueToothplugin::receiveConnectdHotspotChange);

    m_service->moveToThread(m_Thread);
    m_Thread->start();
    this->setObjectName("blueToothPlugin");
}

BlueToothplugin::~BlueToothplugin()
{
    m_Thread->exit();
}

bool BlueToothplugin::open(void)
{   
    QString standardOut;
    QString standardError;
    QString program_rm = "rm";
    QStringList arguments_rm;
    arguments_rm<<"deviceList.txt";
    mySystem(program_rm,arguments_rm);

    QString program = "./run_bluet.sh";
    QStringList arguments;

    arguments << "open";
    if(!mySystem(program,arguments))
    {
        qWarning()<<"BlueTooth:open error "<<standardOut<<standardError;
    }
    QThread::msleep(20);

    //寮€鍚壂鎻?
    //./bluet_oper  scan  > log.txt  &
    arguments.clear();
    arguments << "scan" ;//<< ">" << "deviceList.txt" ;//<< "&";

    // 鍚姩绋嬪簭
    // if(!mySystem(program,arguments))
    // {
    //     qWarning()<<"BlueTooth:open error "<<standardOut<<standardError;
    // }
    m_scanProcess->setStandardOutputFile("deviceList.txt");
    m_scanProcess->start(program, arguments);
    if (!m_scanProcess->waitForStarted()) {
        qDebug() << "Failed to start the program!";
        return false;
    }
    QThread::msleep(20);

    m_Timer->start();
    m_enable = true;
    m_service->resetDataIndex();
    emit scanHotspot();
    emit scanning(true);
    return true;
}

bool BlueToothplugin::close(void)
{
    QString standardOut;
    QString standardError;
    m_enable = false;
    breakConnecting();
    m_hotspotList.clear();
    m_service->clearHotspotList();

    QString program = "./run_bluet.sh";
    QStringList arguments;

    arguments << "close";
    if(!mySystem(program,arguments))
    {
        qWarning()<<"BlueTooth:close error "<<standardOut<<standardError;
    }
    QThread::msleep(20);
    emit refresh();

    m_scanProcess->terminate(); // 鍙戦€佺粓姝俊鍙?

    // 绛夊緟绋嬪簭缁撴潫
    if (!m_scanProcess->waitForFinished()) {
        qDebug() << "Failed to finish!";
        m_Timer->stop();
        return false;
    }
    m_Timer->stop();

    return true;
}

bool BlueToothplugin::connectHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot)
{
    if(!hotspot.m_ssid.isEmpty())
    {
        m_Timer->stop();
        m_service->setbreakConnecting(false);
        emit connectNet(hotspot);
        m_Timer->start();
        return true;
    }
    return false;
}

void BlueToothplugin::scanTimeout(void)
{
    qDebug()<<"---scanTimeout";
    emit scanHotspot();
    emit scanning(true);
}


QList<HOTSPOTINFO_SINGLE_BLUETOOTH> BlueToothplugin::BlueToothList()
{
    return m_hotspotList;
}

bool BlueToothplugin::mySystem(QString program, QStringList arguments, QString *ret_out, QString *ret_error)
{
    int iErr = 0;
    QProcess process;
    qDebug()<<"---program = "<< program <<"arguments = "<<arguments;
    if(ret_out)
    {
        ret_out->clear();
    }

    if(ret_error)
    {
        ret_error->clear();
    }

    if(program.isEmpty())
    {
        if(ret_error)
        {
            ret_error->append("program cannot be empty");
        }
        iErr = -1;
    }

    if(!iErr)
    {
        process.start(program,arguments);
        while(!process.waitForFinished(300) && !terminateProcess)       //绛夊緟鍛戒护鎵ц缁撴潫
        {
            ;
        }

        if(terminateProcess)
        {
            terminateProcess = false;
        }

        if(process.exitStatus() == QProcess::NormalExit)
        {
            if(ret_out)
            {
                ret_out->append(process.readAllStandardOutput());
            }

            if(ret_error)
            {
                ret_error->append(process.readAllStandardError());
            }
            return true;
        }
        else
        {
            if(ret_error)
            {
                ret_error->append(process.readAllStandardError());
            }
            qCritical()<<program<<arguments<<"The process crashed";
        }
    }
    return false;
}

int BlueToothplugin::myRegExp(const QString pattern, const QString &text, QStringList &list)
{
    QRegExp regExp;
    int capCount = 0;
    int pos = 0;

    if(pattern.isEmpty() || text.isEmpty())
    {
        return -1;
    }

    regExp.setPattern(pattern);
    regExp.indexIn(text);

    list.clear();
    while ((pos = regExp.indexIn(text, pos)) != -1)
    {
        list.push_back(regExp.cap(0));
        pos += regExp.matchedLength();
        capCount++;
    }

    return capCount;
}

HOTSPOTINFO_SINGLE_BLUETOOTH BlueToothplugin::getConnectedHotspot(void)
{ 
    return m_service->m_connectedHotspot;
}

void BlueToothplugin::clearConnectedHotspot()
{
    m_service->m_connectedHotspot.m_status = false;
    m_service->m_connectedHotspot.m_bssid = "";
}

/**
    * @brief:涓柇杩炴帴
    * @param[in]:
    * @return:
    * @note:
*/
void BlueToothplugin::breakConnecting()
{
    m_service->setbreakConnecting(true);
    disconnectHotspot();
    return;
}

void BlueToothplugin::refreshList(void)
{
    emit scanHotspot();
    emit scanning(true);
    //m_Timer->start();
}

/**
* @brief:鏂紑杩炴帴
* @param[in]:
* @return:
* @note:
*/
void BlueToothplugin::disconnectHotspot()
{
    QString command = "./run_bluet.sh";
    QString standardOut;
    QString standardError;
    QStringList temp;

    HOTSPOTINFO_SINGLE_BLUETOOTH connectedHotspot;
    connectedHotspot = getConnectedHotspot();
    if(connectedHotspot.m_bssid != "")
    {

        //蹇界暐璁惧
        //./run_bluet.sh ignore 60:F4:3A:40:77:FA
        m_Timer->stop();
        BlueToothplugin::mySystem(command,
                             QStringList()<<"disconnect"<<connectedHotspot.m_bssid,
                             &standardOut,&standardError);
        if(0 != myRegExp("successfully",
                    standardOut,
                    temp))
        {
            clearConnectedHotspot();
            m_service->scan();
            emit state(BlueTooth_DISCONNECT);
        }
        qDebug()<<"disconnect standardOut = "<<standardOut;
        m_Timer->start();
        terminateProcess = true;
    }

    return;
}

void BlueToothplugin::renameHotspot(QString name)
{
    QString command = "./run_bluet.sh";
    QString standardOut;
    QString standardError;
    HOTSPOTINFO_SINGLE_BLUETOOTH connectedHotspot;
    connectedHotspot = getConnectedHotspot();
    if(connectedHotspot.m_bssid != "")
    {
        m_Timer->stop();
        BlueToothplugin::mySystem(command,
                             QStringList()<<"rename"<<connectedHotspot.m_bssid<<name,
                             &standardOut,&standardError);
        m_service->scan();
        m_Timer->start();
    }
}

void BlueToothplugin::ignoreHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH hotspot)
{
    QString command = "./run_bluet.sh";
    QString standardOut;
    QString standardError;
    QStringList temp;

    qDebug() << "ignoreHotspot====================================== ";

    if(hotspot.m_bssid != "")
    {
        m_Timer->stop();
        BlueToothplugin::mySystem(command,
                             QStringList()<<"ignore"<<hotspot.m_bssid,
                             &standardOut,&standardError);

        qDebug() << "standardOut: " << standardOut;
        qDebug() << "standardError: " << standardError;

        //if(0 != myRegExp("successfully",
         //           standardOut,
       //             temp))
       // {
          //  qDebug() << "\nignore successfully====================================== ";
             // m_service->setbreakConnecting(true);
           //   clearConnectedHotspot();
              emit state(BlueTooth_IGNORE);
           // emit state(BlueTooth_DISCONNECT);
              
              m_service->scan();
            // refreshList();
       // }
        m_Timer->start();
        terminateProcess = true;
    }

    return;
}

void BlueToothplugin::receiveHotspotList(bool result)
{
    if(result && m_enable)
    {
        m_hotspotList.clear();
        m_hotspotList = m_service->m_hotspotList;
        emit refresh();
    }
    emit scanning(false);
}

void BlueToothplugin::receiveConnectState(BlueTooth_STATUS connectState, BlueTooth_ERROR connectError)
{
   m_BlueToothState = connectState;
   qDebug()<<"receiveConnectState";
   if(m_enable)
   {
       emit state(m_BlueToothState);
       emit error(connectError);

       if(connectError != BlueTooth_NONE)
       {
//            qDebug()<<"wifi State: "<<m_WiFiState<<"Error: "<<connectError<<
//                      "(2:韬唤楠岃瘉鍑洪敊 3:鑾峰彇IP鍑洪敊 4:鏈煡閿欒)";
       }

//       if(m_WiFiState == WIFI_CONNECT || connectError != WIFI_NONE)
//       {
//           m_Timer->start();
//       }
   }
}

void BlueToothplugin::receiveConnectdHotspotChange()
{
    emit connectedHotspotChange(getConnectedHotspot());
}
/*---------------------------------------------------BlueToothService--------------------------------*/

void BlueToothService::list(void)
{


    printf("list ====================================================== \n");

    QString command = "./run_bluet.sh";
    QString standardOut;
    standardOut.clear();
    QString standardError;
    QStringList hotspotList;
    QStringList hotspotList_1;
    QStringList hotspotSingle;
    int iErr = 0;

    if(!BlueToothplugin::mySystem(command,
                             QStringList()<<"list",
                             &standardOut,&standardError))
    {
        iErr = -1;
    }

    if(iErr)
    {
        qWarning()<<"BlueTooth: list deviceList"<<standardOut<<standardError;
    }

    if(!iErr)
    {
        if(1 <= BlueToothplugin::myRegExp("[^\n]+",
                                     standardOut,hotspotList))
        {
            printf("list match ================================= hotspotList.size = %d\n",hotspotList.size());
            for(int i = 0; i < hotspotList.size();i++)
            {
                if(!BlueToothplugin::myRegExp("\t",
                                     hotspotList.at(i),hotspotList_1))
                {
                    qDebug()<<"!BlueToothplugin::myRegExp('\t',standardOut,hotspotList)";
                    continue;
                }

                QString tempText = hotspotList.at(i);
                hotspotSingle.clear();
                QStringList temp;
                hotspotSingle = tempText.split('\t');
                
                for(int j = 0;j < hotspotSingle.size();j++)
                {
                    qDebug()<<"hotspotSingle = "<< hotspotSingle.at(i);
                }
                HOTSPOTINFO_SINGLE_BLUETOOTH hotspotInfo;
                hotspotInfo.m_property = hotspotSingle.at(0);
                hotspotInfo.m_ssid = hotspotSingle.at(1);
                hotspotInfo.m_bssid = hotspotSingle.at(2);
                if(0 != BlueToothplugin::myRegExp("not",
                                             hotspotSingle.at(3),
                                             temp))
                {
                    hotspotInfo.m_orderLevel = 2;
                    hotspotInfo.m_status = false;
                    if((!QString::compare(m_connectedHotspot.m_bssid,hotspotInfo.m_bssid)) && m_connectedHotspot.m_status)
                    {
                        m_connectedHotspot = hotspotInfo;
                        emit connectedHotspotChange();
                        emit connectState(BlueTooth_DISCONNECT,BlueTooth_NONE);
                    }
                }
                else
                {   
                    hotspotInfo.m_orderLevel = 1;
                    hotspotInfo.m_status = true;
                    if(!m_connectedHotspot.m_status)
                    {
                        m_connectedHotspot = hotspotInfo;
                        emit connectedHotspotChange();
                        emit connectState(BlueTooth_CONNECT,BlueTooth_NONE);
                    }
                }
                qDebug()<<" ---hotspotInfo = "<< hotspotInfo.m_property << hotspotInfo.m_ssid<< hotspotInfo.m_bssid<< hotspotInfo.m_status;
                m_hotspotList.append(hotspotInfo);
            }
        }

    }
}

void BlueToothService::scan(void)
{
    //浼樺厛鎵цlist鍛戒护,鑾峰彇宸茶繛鎺ヨ澶?

    list();

    printf("scan ================================\n");

    QString command = "cat";
    QString standardOut;
    standardOut.clear();
    QString standardError;
    QStringList hotspotList;
    QStringList hotspotList_1;
    QStringList hotspotSingle;
    int iErr = 0;

    //checkConnectState();
    if(!BlueToothplugin::mySystem(command,
                             QStringList()<<"deviceList.txt",
                             &standardOut,&standardError))
    {
        iErr = -1;
    }

    if(standardOut.isEmpty())
    {
        iErr = -2;
    }

    if(iErr)
    {
        qWarning()<<"BlueTooth: cat deviceList"<<standardOut<<standardError;
        return;
    }
    qDebug()<<"iErr = "<<iErr;
    if(!iErr)
    {
        if(1 <= BlueToothplugin::myRegExp("[^\n]+",
                                     standardOut,hotspotList))
        {


           printf("scan match ==============================  hotspotList.size:%d\n",hotspotList.size());

            //m_hotspotList.clear();
            unsigned int curdataIndex = m_curdataIndex;
            for(int i = curdataIndex; i < hotspotList.size(); i++)
            {
                if(!BlueToothplugin::myRegExp("\t",
                                     hotspotList.at(i),hotspotList_1))
                {
                    qDebug()<<"scan myRegExp '\t' t error!";
                    m_curdataIndex++;
                    continue;
                }
                QString tempText = hotspotList.at(i);
                hotspotSingle.clear();
                QStringList temp;
                hotspotSingle = tempText.split('\t');
                HOTSPOTINFO_SINGLE_BLUETOOTH hotspotInfo;
                hotspotInfo.m_property = hotspotSingle.at(1);
                hotspotInfo.m_ssid = hotspotSingle.at(2);//hexTransforChar(hotspotSingle.at(2));
                hotspotInfo.m_bssid = hotspotSingle.at(3);
                hotspotInfo.m_status = false;
                hotspotInfo.m_orderLevel = 3;
                if(0 != BlueToothplugin::myRegExp("new",
                                             hotspotSingle.at(1),
                                             temp))
                {
                    m_hotspotList.append(hotspotInfo);
                }
                else if(0 != BlueToothplugin::myRegExp("del",
                                                  hotspotSingle.at(1),
                                                  temp))
                {
                    for(int k = 0;k<m_hotspotList.size();)
                    {

                        if(!QString::compare(m_hotspotList.at(k).m_bssid,hotspotInfo.m_bssid))
                        {
                            m_hotspotList.removeAt(k);
                        }else
                        {
                            k++;
                        }
                    }
                }
                m_curdataIndex++;
                //m_hotspotList.append(hotspotInfo);
            }
            filter(m_hotspotList);    // filter first
            emit scanResult(true);
        }
        else{
            qDebug()<<"scan myRegExp error!";
        }
    }
    else
    {
        emit scanResult(false);
    }
}

bool BlueToothService::getbreakConnecting(void)
{
    bool ret;
    m_Mutex.lock();
    ret = m_breakConnecting;
    m_Mutex.unlock();
    return ret;
}

void BlueToothService::setbreakConnecting(bool value)
{
    m_Mutex.lock();
    m_breakConnecting = value;
    m_Mutex.unlock();
}

/**
* @brief:杩炴帴缃戠粶(鍖呮嫭韬唤楠岃瘉鍜岃幏鍙朓P鍦板潃)
* @param[in]:
* @return:
* @note:
*/

void BlueToothService::connectNetwork(HOTSPOTINFO_SINGLE_BLUETOOTH hotspot)
{
    QString command = "./run_bluet.sh";
    QString standardOut;
    QString standardError;
    QStringList stringList;
    QStringList devStringList;
    QStringList hotspotList;
    QStringList hotspotList_1;
    QStringList hotspotSingle;
    QString net_num;
    int iErr = 0;

    if(getConnectingFlag())
    {
        return;
    }

    emit connectState(BlueTooth_CONNECTING,BlueTooth_NONE);
    setConnectingFlag(true);

    //m_connectedHotspot = hotspot;  // need early `case refresh red item on the top
    //emit connectState(WIFI_AUTHENTICATION,WIFI_NONE);

    if(!iErr)
    {
        BlueToothplugin::mySystem(command,
                             QStringList()<<"connect"<<hotspot.m_bssid,
                             &standardOut,&standardError);
        if(standardOut.isEmpty() || !standardError.isEmpty())
        {
            iErr = -1;
        }
    }
    if(iErr < 0)
    {
        iErr = 1;
        emit connectState(BlueTooth_CONNECTFAIL,BlueTooth_NONE);
        qWarning()<<"buletooth: ./run_bluet.sh connect"<< "standardOut = " << standardOut << "standardError = " <<standardError;
    }

    if(getbreakConnecting())
    {
        setConnectingFlag(false);
        return;
    }
    if(!iErr)
    {
        int loop = 20;
        while(1)
        {
            if(getbreakConnecting())
            {
                setConnectingFlag(false);
                return;
            }

            BlueToothplugin::mySystem(command,
                                 QStringList()<<"list",
                                 &standardOut,&standardError);
            if(!standardOut.isEmpty())
            {
                if(1 <= BlueToothplugin::myRegExp("[^\n]+",
                                     standardOut,hotspotList))
                {
                    for(int i = 0; i < hotspotList.size(); i++)
                    {
                        if(!BlueToothplugin::myRegExp("\t",
                                            hotspotList.at(i),hotspotList_1))
                        {
                            break;
                        }
                        QString tempText = hotspotList.at(i);
                        hotspotSingle.clear();
                        QStringList temp;
                        hotspotSingle = tempText.split('\t');
                        qDebug()<<"------hotspot.m_bssid = "<<hotspot.m_bssid <<",  " << hotspotSingle.at(2);
                        if(!QString::compare(hotspot.m_bssid,hotspotSingle.at(2)))
                        {
                            qDebug()<<"------hotspotSingle.at(3) = "<<hotspotSingle.at(3);
                            if(0 == BlueToothplugin::myRegExp("not",
                                             hotspotSingle.at(3),
                                             stringList))
                            //if(BlueToothplugin::myRegExp("connected",standardOut,stringList))
                            {
                                qDebug()<<"------------connected";
                                hotspot.m_status = true;
                                m_connectedHotspot = hotspot;
                                emit connectState(BlueTooth_CONNECT,BlueTooth_NONE);
                                scan();
                                setConnectingFlag(false);
                                return;
                            }
                        }
                    }
                }
                 
            }

            QThread::msleep(400);

            if(0 == loop--)
            {
                //韬唤楠岃瘉澶辫触
                emit connectState(BlueTooth_CONNECTFAIL,BlueTooth_NONE);
                setConnectingFlag(false);
                return;
            }
        }
        setConnectingFlag(false);
    }

    if(iErr)
    {
        //emit connectState(WIFI_AUTHENTICATION,WIFI_UNKNOWNERROR);
        setConnectingFlag(false);
        return;
    }

}

bool BlueToothService::compareLevel(const HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot1, const HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot2)
{
    return hotspot1.m_orderLevel < hotspot2.m_orderLevel;
}

void BlueToothService::filter(QList<HOTSPOTINFO_SINGLE_BLUETOOTH> &list)
{
    HOTSPOTINFO_SINGLE_BLUETOOTH Temp;
    int i=0;
    if(list.count() <= 1)
    {
        return;
    }

    /**
    *鍘绘帀閲嶅鐨勮澶?
    */
    for(i = 0; i < list.count(); i++)
    {
        Temp = list.at(i);
        for(int j = i+1; j < list.count(); j++)
        {
            if(!QString::compare(Temp.m_bssid,list.at(j).m_bssid))
            {
                list.removeAt(i);
                // if(Temp.m_status)
                // {
                //     list.removeAt(j);
                // }
                // else
                // {
                //     list.removeAt(i);
                // }
                i--;
                continue;
            }
        }
    }

    // 鎸夌収椤哄簭鎺掑簭
    qSort(list.begin(), list.end(), compareLevel);
}

bool BlueToothService::getConnectingFlag()
{
    return m_connectingFlag;
}

void BlueToothService::setConnectingFlag(bool flag)
{
    m_connectingFlag = flag;
}
