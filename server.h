#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSharedPointer>
#include <QThread>
#include <QMap>

#include "ServerList/serverlist.h"
#include "Block/AddressBlock/addressblock.h"
#include "ClientHandler/clienthandler.h"
#include "DbFileFragInfo/dbcache.h"
#include "DbAddressPath/dbaddresspathbyfile.h"
#include "FileBlockStorage/filefrag.h"

namespace Cuma
{

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(unsigned int Port,
                    QSharedPointer<Cuma::NetworkConfig::ServerList>& list,
                    QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& info,
                    QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressByFile,
                    QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage);

    ~Server();

signals:
    void Stop();

    void Start();

public slots:
    void OnConnect();

    void OnStop();

    void OnStart();

protected:
    void BindThreadClientHandler(const QSharedPointer<Cuma::ClientHandler>& Handler, const QSharedPointer<QThread>& Thread);

private:
    QSharedPointer<QTcpServer> ConnectRequestServer;

    QMap<QString, QSharedPointer<Cuma::ClientHandler> > RunningClientHandler;

    QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList;

    QVector<QSharedPointer<QThread>> ClientHandlerThreadList;

    unsigned int port;

private:
    QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressByFile;

    QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage;

    QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo;

};

}

#endif // SERVER_H
