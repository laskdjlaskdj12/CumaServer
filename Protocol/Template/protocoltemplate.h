#ifndef PROTOCOLTEMPLATE_H
#define PROTOCOLTEMPLATE_H

#include <QObject>

#include <QJsonObject>
#include <QJsonDocument>

#include "Block/AddressBlock/addressblock.h"
#include "Protocol/Type/protocoltype.h"
#include "Protocol/protocol.h"

namespace Cuma
{
namespace  Protocol
{

class Template : public QObject
{
    Q_OBJECT
public:

    static QJsonObject RequestConnect(const Cuma::Address::AddressBlock& Address);

    static QJsonObject RequestSpread(const Cuma::Address::AddressBlock& Address, const QByteArray Data);

    static QJsonObject RequestSearch(const Cuma::Address::AddressBlock& Address, const QByteArray Data);

    static QJsonObject RequestDownload(const Cuma::Address::AddressBlock& Address, const QByteArray Data);

    static QJsonObject RequestBypass(const Cuma::Address::AddressBlock& Address, const QByteArray Data, bool isSessionConnect);

    static Cuma::Protocol::CumaProtocolBlock ReplyConnect(const QJsonObject& obj);

    static Cuma::Protocol::CumaProtocolBlock ReplySpread(const QJsonObject& obj);

    static Cuma::Protocol::CumaProtocolBlock ReplySearch(const QJsonObject& obj);

    static Cuma::Protocol::CumaProtocolBlock ReplyDownload(const QJsonObject& obj);

    static Cuma::Protocol::CumaProtocolBlock ReplyBypass(const Cuma::Address::AddressBlock& Address, const QByteArray Data);

};
}
}

#endif // PROTOCOLTEMPLATE_H
