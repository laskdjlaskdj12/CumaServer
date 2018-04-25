#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QCryptographicHash>

#include "JsonSocketLib/qtjsonsocketlib_v3.h"

#include "ServerList/serverlist.h"

#include "DbAddressPath/dbaddresspathbyfile.h"

#include "DbFileFragInfo/dbcache.h"

#include "FileBlockStorage/filefrag.h"

#include "Download/downloadhandler.h"
#include "Spread/spreadhandler.h"
#include "Search/searchhandler.h"

class ClientController : public QObject
{
    Q_OBJECT
public:
    explicit ClientController(QSharedPointer<QtJsonSocketLib_v3>& client,
                              QSharedPointer<Cuma::NetworkConfig::ServerList>& ServerList,
                              QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                              QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                              QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage,
                              QCryptographicHash::Algorithm Algorithm);
    ~ClientController();

    void ReplyControl(Cuma::Protocol::CumaProtocolBlock RecvProtocol);


protected:
    bool SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

    Cuma::Protocol::CumaProtocolBlock RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client);

    void ConnectHandler(Cuma::Protocol::CumaProtocolBlock Block);

private:
    QSharedPointer<QtJsonSocketLib_v3>& Client;
    QSharedPointer<Cuma::NetworkConfig::ServerList>& ServerList;
    QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath;
    QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo;
    QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage;
    QCryptographicHash::Algorithm Algorithm;
};

#endif // CLIENTCONTROLLER_H
