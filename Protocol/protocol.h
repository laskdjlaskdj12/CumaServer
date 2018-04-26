#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Block/AddressBlock/addressblock.h"
#include "Type/protocoltype.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Cuma
{
namespace Protocol
{

typedef struct Protocol
{
    Protocol() {}
    Protocol(const Protocol& a)
    {
        Address = a.Address;
        ProtocolType = a.ProtocolType;
        Data = a.Data;
    }

    void operator = (const Protocol& a)
    {
        Address = a.Address;
        ProtocolType = a.ProtocolType;
        Data = a.Data;
    }

    Cuma::Address::AddressBlock Address;
    Cuma::Protocol::Type ProtocolType;
    QByteArray Data;
} CumaProtocolBlock;


class Serlize : public QObject
{
    Q_OBJECT
public:
    static QJsonObject SerlizeProtocol(CumaProtocolBlock Block)
    {
        QJsonObject ToAddress;
        ToAddress["IP"] = static_cast<QString>(Block.Address.To.IP);
        ToAddress["Port"] = static_cast<int>(Block.Address.To.Port);

        QJsonObject FromAddress;
        FromAddress["IP"] = static_cast<QString>(Block.Address.From.IP);
        FromAddress["Port"] = static_cast<int>(Block.Address.From.Port);

        QJsonObject Result;

        Result["Bypass"] = Block.Address.BypassArray;
        Result["Direction"] = static_cast<int>(Block.Address.Direction);
        Result["PathCount"] = static_cast<int>(Block.Address.PathCount);
        Result["From"] = FromAddress;
        Result["To"] = ToAddress;
        Result["Protocol"] = Block.ProtocolType;
        Result["Data"] = static_cast<QString>(Block.Data.toBase64());

        return Result;
    }

    static CumaProtocolBlock DeSerlizeProtocol(QJsonObject Block)
    {
        Cuma::Address::IpAddress ToAddress;
        ToAddress.IP = Block["To"].toObject()["IP"].toString();
        ToAddress.Port = static_cast<int>(Block["To"].toObject()["Port"].toInt());

        Cuma::Address::IpAddress FromAddress;
        FromAddress.IP = static_cast<QString>(Block["From"].toObject()["IP"].toString());
        FromAddress.Port = static_cast<int>(Block["From"].toObject()["Port"].toInt());

        Cuma::Protocol::CumaProtocolBlock Result;
        Result.Address.From = FromAddress;
        Result.Address.To = ToAddress;
        Result.Address.BypassArray = Block["Bypass"].toArray();
        Result.Address.Direction = static_cast<Cuma::Address::BypassDirection>(Block["Direction"].toInt());
        Result.Address.PathCount = Block["PathCount"].toInt();
        Result.ProtocolType = static_cast<Cuma::Protocol::Type>(Block["Protocol"].toInt());
        Result.Data = QByteArray::fromBase64(Block["Data"].toString().toUtf8());

        return Result;
    }

};

}
}
#endif // PROTOCOL_H
