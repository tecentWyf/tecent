#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include <QTimer>
#include <QProcess>
#include <QStringList>
#include <QObject>
#include <QThread>
#include <QtSql>
#include "blueToothplugininterface.h"
class BlueToothService;

class BlueToothplugin : public BlueToothPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(BlueToothPluginInterface)
    Q_PLUGIN_METADATA(IID BlueToothPluginInterface_iid)
public:
    BlueToothplugin();
    ~BlueToothplugin();

    virtual bool open(void);
    virtual bool close(void);

    virtual QList<HOTSPOTINFO_SINGLE_BLUETOOTH> BlueToothList(void);
    virtual bool connectHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot);
    virtual HOTSPOTINFO_SINGLE_BLUETOOTH getConnectedHotspot(void);

    static int myRegExp(const QString pattern, const QString &text, QStringList &list);
    static bool mySystem(QString program, QStringList arguments,
                  QString *ret_out = NULL, QString *ret_error = NULL);

    void clearConnectedHotspot();
public slots:
    virtual void disconnectHotspot(void);
    virtual void breakConnecting(void);
    virtual void refreshList(void);
    virtual void renameHotspot(QString name);
    virtual void ignoreHotspot(HOTSPOTINFO_SINGLE_BLUETOOTH);
private slots:
    void receiveHotspotList(bool);
    void scanTimeout(void);
    void receiveConnectState(BlueTooth_STATUS, BlueTooth_ERROR);
    void receiveConnectdHotspotChange();
signals:
    void scanHotspot(void);
    void connectNet(HOTSPOTINFO_SINGLE_BLUETOOTH);

private:
    QList<HOTSPOTINFO_SINGLE_BLUETOOTH> m_hotspotList;
    BlueToothService *m_service;
    BlueTooth_STATUS m_BlueToothState;
    QTimer *m_Timer;
    QThread *m_Thread;
    QProcess *m_scanProcess;
    bool    m_enable;
};

class BlueToothService : public QObject
{
    Q_OBJECT
public:
    BlueToothService()
        :m_breakConnecting(false)
        ,m_isConnected(false)
        ,m_curdataIndex(0)
        ,m_connectingFlag(false)
    {
        m_hotspotList.clear();
    }
    bool getIPAddress(QString &IPAddress);
    void resetDataIndex()
    {
        m_curdataIndex = 0;
    }
    void list(void);
    void clearHotspotList()
    {
        m_hotspotList.clear();
    }

    bool getConnectingFlag();
    void setConnectingFlag(bool flag);
public slots:
    void scan(void);
    void connectNetwork(HOTSPOTINFO_SINGLE_BLUETOOTH hotspot);
    void setbreakConnecting(bool);
    bool getbreakConnecting(void);


signals:
    void scanResult(bool);
    void connectState(BlueTooth_STATUS, BlueTooth_ERROR);
    void connectedHotspotChange();
public:
    QList<HOTSPOTINFO_SINGLE_BLUETOOTH> m_hotspotList;
    HOTSPOTINFO_SINGLE_BLUETOOTH m_connectedHotspot;

private:
    static bool compareLevel(const HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot1, const HOTSPOTINFO_SINGLE_BLUETOOTH &hotspot2);
    void filter(QList<HOTSPOTINFO_SINGLE_BLUETOOTH> &list);
    QString hexTransforChar(QString str);                        // 用于utf编码的中文十六进制字符串，转换成中文字符编码
    QString gbk2utf(char *ch);                                   // GBK编码转utf-8编码
    QString codeConvert(char *source_charset, char *to_charset, char *sourceStr);


    bool m_breakConnecting;
    bool m_isConnected;
    QMutex m_Mutex;
    unsigned int  m_curdataIndex;
    bool    m_connectingFlag;
};
#endif // BLUETOOTHPLUGIN_H
