#ifndef ERRORBLOCK_H
#define ERRORBLOCK_H

#include <QObject>

namespace Cuma
{

namespace Network
{

namespace Client
{
enum Error : uint8_t
{
    None,
    NOResultProtocol,
    ReplyBlockEmpty,
    EventLoopStart,
    RequestBlockEmpty,
    ServerListEmpty,
    FileFragCount,
    FileOpenError,
    FileNameEmpty,
    FileSourceEmpty,
    FileFragSaveError,
    FileFragAddressDbAddFail,
    FileFragAddressEmpty,
    FileRecvInfoError,
    ProtocolSerlize,
    ProtocolDeSerlize,
    ProtocolUnknown,
    ParseFromData,
    SpreadFail,
    FileCast,
    JsonCast,
    Disconnect,
    ConnectRefuse,
    Connect,
    BypassConnect,
    Socket,
    Timeout,
    Send,
    Recv,
    ResultAddConnectServerInfo,
    ResultAddConnectionBypassChainServerInfo,
    ResultFileSourceEmpty,
    ResultFileFragSaveError,
    ResultFileFragDbSsaveError,
    ResultAddAddressPathOfFileDb
};
}

namespace Server
{
enum Error : uint8_t
{
    None,
    CreateSocket,
    NOResultProtocol,
    ReplyBlockEmpty,
    EventLoopStart,
    RequestBlockEmpty,
    ServerListEmpty,
    FileFragCount,
    FileOpenError,
    FileNameEmpty,
    FileSourceEmpty,
    FileFragSaveError,
    FileFragAddressDbAddFail,
    FileFragAddressEmpty,
    FileRecvInfoError,
    ProtocolSerlize,
    ProtocolDeSerlize,
    ProtocolUnknown,
    ParseFromData,
    SpreadFail,
    FileCast,
    JsonCast,
    Disconnect,
    ConnectRefuse,
    Connect,
    BypassConnect,
    Socket,
    Timeout,
    Send,
    Recv,
    ResultAddConnectServerInfo,
    ResultAddConnectionBypassChainServerInfo,
    ResultFileSourceEmpty,
    ResultFileFragSaveError,
    ResultFileFragDbSsaveError,
    ResultAddAddressPathOfFileDb
};

}

}

}
#endif // ERRORBLOCK_H
