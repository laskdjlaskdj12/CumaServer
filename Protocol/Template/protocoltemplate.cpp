#include "protocoltemplate.h"

QJsonObject Cuma::Protocol::Template::RequestConnect(const Cuma::Address::AddressBlock& Address)
{
    QJsonObject ToAddress;
    ToAddress["IP"] = Address.To.IP;
    ToAddress["Port"] = static_cast<int>(Address.To.Port);

    QJsonObject RetObj;
    RetObj["BypassArray"] = Address.BypassArray;
    RetObj["Direction"] = static_cast<int>(Address.Direction);
    RetObj["To"] = ToAddress;
    RetObj["Protocol"] = Cuma::Protocol::Type::Connect;
    RetObj["BypassCount"] = static_cast<int>(Address.PathCount);

    return RetObj;
}

QJsonObject Cuma::Protocol::Template::RequestSpread(const Cuma::Address::AddressBlock& Address, const QByteArray Data)
{
    QJsonObject ToAddress;
    ToAddress["IP"] = Address.To.IP;
    ToAddress["Port"] = static_cast<int>(Address.To.Port);

    QJsonObject RetObj;
    RetObj["BypassArray"] = Address.BypassArray;
    RetObj["Direction"] = static_cast<int>(Address.Direction);
    RetObj["To"] = ToAddress;
    RetObj["BypassCount"] = static_cast<int>(Address.PathCount);
    RetObj["Protocol"] = Cuma::Protocol::Type::Spread;
    RetObj["Data"] = QString(Data.toBase64());

    return RetObj;
}

QJsonObject Cuma::Protocol::Template::RequestSearch(const Cuma::Address::AddressBlock& Address, const QByteArray Data)
{
    QJsonObject ToAddress;
    ToAddress["IP"] = Address.To.IP;
    ToAddress["Port"] = static_cast<int>(Address.To.Port);

    QJsonObject RetObj;
    RetObj["BypassArray"] = Address.BypassArray;
    RetObj["Direction"] = static_cast<int>(Address.Direction);
    RetObj["To"] = ToAddress;
    RetObj["BypassCount"] = static_cast<int>(Address.PathCount);
    RetObj["Protocol"] = Cuma::Protocol::Type::Search;
    RetObj["Data"] = QString(Data.toBase64());

    return RetObj;
}

QJsonObject Cuma::Protocol::Template::RequestDownload(const Cuma::Address::AddressBlock& Address, const QByteArray Data)
{
    QJsonObject ToAddress;
    ToAddress["IP"] = Address.To.IP;
    ToAddress["Port"] = static_cast<int>(Address.To.Port);

    QJsonObject RetObj;
    RetObj["BypassArray"] = Address.BypassArray;
    RetObj["Direction"] = static_cast<int>(Address.Direction);
    RetObj["To"] = ToAddress;
    RetObj["BypassCount"] = static_cast<int>(Address.PathCount);
    RetObj["Protocol"] = Cuma::Protocol::Type::Download;
    RetObj["Data"] = QString(Data.toBase64());

    return RetObj;
}

QJsonObject Cuma::Protocol::Template::RequestBypass(const Cuma::Address::AddressBlock& Address, const QByteArray Data, bool isSessionConnect)
{
    QJsonObject ToAddress;
    ToAddress["IP"] = Address.To.IP;
    ToAddress["Port"] = static_cast<int>(Address.To.Port);

    QJsonObject RetObj;
    RetObj["BypassArray"] = Address.BypassArray;
    RetObj["Direction"] = static_cast<int>(Address.Direction);
    RetObj["To"] = ToAddress;
    RetObj["BypassCount"] = static_cast<int>(Address.PathCount);
    RetObj["Protocol"] = Cuma::Protocol::Type::Bypass;
    RetObj["Data"] = QString(Data.toBase64());

    return RetObj;
}

Cuma::Protocol::CumaProtocolBlock Cuma::Protocol::Template::ReplyConnect(const QJsonObject& obj)
{
    Cuma::Protocol::CumaProtocolBlock  Block;
    Block.Address.BypassArray = obj["BypasaArray"].toArray();
    Block.Address.Direction = static_cast<Cuma::Address::BypassDirection>(obj["Direction"].toInt());
    Block.Address.PathCount = obj["BypassCount"].toInt();

    Block.Address.From.IP = obj["From"].toObject()["IP"].toString();
    Block.Address.From.Port = obj["From"].toObject()["Port"].toInt();

    Block.Address.To.IP = obj["To"].toObject()["IP"].toString();
    Block.Address.To.Port = obj["To"].toObject()["Port"].toInt();

    Block.ProtocolType = static_cast<Cuma::Protocol::Type>(obj["Protocol"].toInt());
    Block.Data = QByteArray::fromBase64(QString(obj["Data"].toString()).toUtf8());


    return Block;
}

Cuma::Protocol::CumaProtocolBlock Cuma::Protocol::Template::ReplySpread(const QJsonObject& obj)
{
    Cuma::Protocol::CumaProtocolBlock  Block;
    Block.Address.BypassArray = obj["BypasaArray"].toArray();
    Block.Address.Direction = static_cast<Cuma::Address::BypassDirection>(obj["Direction"].toInt());
    Block.Address.PathCount = obj["BypassCount"].toInt();

    Block.Address.From.IP = obj["From"].toObject()["IP"].toString();
    Block.Address.From.Port = obj["From"].toObject()["Port"].toInt();

    Block.Address.To.IP = obj["To"].toObject()["IP"].toString();
    Block.Address.To.Port = obj["To"].toObject()["Port"].toInt();

    Block.ProtocolType = static_cast<Cuma::Protocol::Type>(obj["Protocol"].toInt());
    Block.Data = QByteArray::fromBase64(QString(obj["Data"].toString()).toUtf8());


    return Block;
}

Cuma::Protocol::CumaProtocolBlock Cuma::Protocol::Template::ReplySearch(const QJsonObject& obj)
{
    Cuma::Protocol::CumaProtocolBlock  Block;
    Block.Address.BypassArray = obj["BypasaArray"].toArray();
    Block.Address.Direction = static_cast<Cuma::Address::BypassDirection>(obj["Direction"].toInt());
    Block.Address.PathCount = obj["BypassCount"].toInt();

    Block.Address.From.IP = obj["From"].toObject()["IP"].toString();
    Block.Address.From.Port = obj["From"].toObject()["Port"].toInt();

    Block.Address.To.IP = obj["To"].toObject()["IP"].toString();
    Block.Address.To.Port = obj["To"].toObject()["Port"].toInt();

    Block.ProtocolType = static_cast<Cuma::Protocol::Type>(obj["Protocol"].toInt());
    Block.Data = QByteArray::fromBase64(QString(obj["Data"].toString()).toUtf8());

    return Block;
}

Cuma::Protocol::CumaProtocolBlock Cuma::Protocol::Template::ReplyDownload(const QJsonObject& obj)
{
    Cuma::Protocol::CumaProtocolBlock  Block;
    Block.Address.BypassArray = obj["BypasaArray"].toArray();
    Block.Address.Direction = static_cast<Cuma::Address::BypassDirection>(obj["Direction"].toInt());
    Block.Address.PathCount = obj["BypassCount"].toInt();

    Block.Address.From.IP = obj["From"].toObject()["IP"].toString();
    Block.Address.From.Port = obj["From"].toObject()["Port"].toInt();

    Block.Address.To.IP = obj["To"].toObject()["IP"].toString();
    Block.Address.To.Port = obj["To"].toObject()["Port"].toInt();

    Block.ProtocolType = static_cast<Cuma::Protocol::Type>(obj["Protocol"].toInt());
    Block.Data = QByteArray::fromBase64(QString(obj["Data"].toString()).toUtf8());


    return Block;
}

Cuma::Protocol::CumaProtocolBlock Cuma::Protocol::Template::ReplyBypass(const Cuma::Address::AddressBlock& Address, const QByteArray Data)
{
    Cuma::Protocol::CumaProtocolBlock  Block;
    Block.Address = Address;
    Block.ProtocolType = static_cast<Cuma::Protocol::Type>(Cuma::Protocol::Bypass);
    Block.Data = Data;


    return Block;
}

