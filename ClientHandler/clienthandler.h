#ifndef SERVEREVENTLOOP_H
#define SERVEREVENTLOOP_H


#include <QFutureWatcher>
#include <QFuture>
#include <QThreadPool>
#include <QThread>
#include <QHostAddress>

#include <QJsonObject>
#include <QJsonDocument>

#include <QObject>

#include <QTimer>

#include "ServerList/serverlist.h"
#include "JsonSocketLib/qtjsonsocketlib_v3.h"
#include "Protocol/Type/protocoltype.h"
#include "Protocol/Template/protocoltemplate.h"
#include "Protocol/protocol.h"
#include "Block/AddressBlock/addressblock.h"
#include "Type/ServerError/errorblock.h"
#include "Log/errorlog.h"
#include "Log/debuglog.h"
#include "BypassHandler/bypasshandler.h"

namespace Cuma
{

class ClientHandler: public QObject
{
    Q_OBJECT

public:
    ClientHandler(QSharedPointer<QtJsonSocketLib_v3> client,
                  QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList);
    ~ClientHandler();

public slots:
    void Start();

    void Stop();

    void OnRecv();

    void OnDisconnectPreview();

    void OnDisconnectNext();

protected:
    bool IsRequestBypass(const Cuma::Protocol::CumaProtocolBlock& Block);

    void UpdateBypassInfo(Cuma::Protocol::CumaProtocolBlock& Block);

    QSharedPointer<QtJsonSocketLib_v3> ConnectNextBypassServer(Cuma::Address::IpAddress address);

    void SendConnectResult(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

    void StartBypassSession(QSharedPointer<QtJsonSocketLib_v3> PreviewBypassServer, QSharedPointer<QtJsonSocketLib_v3> NextBypassServer);

    void ConnectHandler(Cuma::Protocol::CumaProtocolBlock Block);

signals:
    void EndProtocol(bool Success, Cuma::Protocol::CumaProtocolBlock ReqProtocol);

    void DisconnectClient(bool Success);

    void DisconnectPreviewServer(bool Success);

    void DisconnectNextServer(bool Success);

protected:
    bool SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

    Cuma::Protocol::CumaProtocolBlock RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client);

    Cuma::Address::IpAddress GetNextBypassAddress(const Cuma::Protocol::CumaProtocolBlock& Block);

private:
    QSharedPointer<QtJsonSocketLib_v3> Client;

    QSharedPointer<NetworkConfig::ServerList> ServerList;

    QSharedPointer<BypassHandler> bypassHandler;

    bool IsBypassBrockerActive;
};
}
#endif // SERVEREVENTLOOP_H
