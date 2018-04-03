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

namespace Cuma
{

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(unsigned int Port,
                    QSharedPointer<Cuma::NetworkConfig::ServerList> list);

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

    QSharedPointer<Cuma::NetworkConfig::ServerList> List;

    QVector<QSharedPointer<QThread>> ClientHandlerThreadList;

    unsigned int port;
};

}

#endif // SERVER_H
