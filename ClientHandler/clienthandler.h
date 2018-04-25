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

#include "DbFileFragInfo/dbcache.h"

#include "DbAddressPath/dbaddresspathbyfile.h"

//ClientController 클래스
#include "ClientHandler/Client/controller.h"

//BypassController 클래스
#include "Bypass/bypasscontroller.h"

namespace Cuma
{

class ClientHandler: public QObject
{
    Q_OBJECT

public:
    ClientHandler(QSharedPointer<QtJsonSocketLib_v3> client,
                  QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList,
                  QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                  QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                  QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage,
                  QCryptographicHash::Algorithm Algorithm);

    ~ClientHandler();

    void ReplyFailControl(const Cuma::Protocol::CumaProtocolBlock& RecvBlock, const QString& str);

    Cuma::Protocol::CumaProtocolBlock RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client);

    bool SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

public slots:
    void Start();

    void Stop();

    void OnRecv();

    //Bypass 영역
protected:
    bool IsRequestBypass(const Cuma::Protocol::CumaProtocolBlock& Block);

    //부모에게 상태 이상 메세지를 보내는 시그널
signals:
    void EndProtocol(bool Success, Cuma::Protocol::CumaProtocolBlock ReqProtocol);

    void DisconnectClient(bool Success);

private:
    QSharedPointer<ClientController> ClientRequestController;

    QSharedPointer<BypassController> ClientRequestBypass;

private:
    QSharedPointer<QtJsonSocketLib_v3> Client;

private:
    QSharedPointer<NetworkConfig::ServerList> ServerList;

    QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath;

    QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo;

    QCryptographicHash::Algorithm Algorithm;

    QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage;

};
}
#endif // SERVEREVENTLOOP_H
