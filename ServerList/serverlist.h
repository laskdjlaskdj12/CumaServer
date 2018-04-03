#ifndef SERVERLIST_H
#define SERVERLIST_H

#include <QObject>
#include <QSharedPointer>
#include <QFile>
#include <QUrl>
#include <QVector>
#include "QDebug"
#include "QFileInfo"
#include "QDir"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>

#include "Block/AddressBlock/addressblock.h"
#include "Log/debuglog.h"

namespace Cuma
{

namespace NetworkConfig
{

class ServerList : public QObject
{
    Q_OBJECT
public:
    ServerList(QString ServerFile);

    ~ServerList();

    virtual bool Add(const QUrl ip, const unsigned int port);

    virtual bool Add(const QJsonArray ServerChain);

    virtual bool Remove(const QJsonArray ServerArray);

    virtual bool Remove(const QUrl ip, const unsigned int port);

    virtual bool RemoveAll();

    virtual bool Exsist(const QJsonArray ServerChain);

    virtual bool Exsist(const QUrl ip, const unsigned int port);

    virtual void Close();

    virtual QVector<QString> GetAllServerList();

    virtual QJsonArray GetAllServerListToArray();

    virtual QVector<Address::IpAddress> GetAllServerIpAddressList();

    bool IsError();

    QString ErrorString();

protected:
    QFile File;

    QString ErrorStr;

    QJsonObject AddressObject;

    bool Error;
};

}

}
#endif // SERVERLIST_H
