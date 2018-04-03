#ifndef ADDRESSBLOCK_H
#define ADDRESSBLOCK_H

#include <QObject>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>

namespace Cuma
{

namespace Address
{

enum BypassDirection : uint8_t
{
    Req,
    Rply
};

struct IpAddress
{
    IpAddress() {}

    IpAddress(const IpAddress& A)
    {
        IP = A.IP;
        Port = A.Port;
    }

    IpAddress(const QString& ip, const unsigned int& port)
    {
        IP = ip;
        Port = port;
    }

    bool operator == (const IpAddress& A)
    {
        return ((IP == A.IP) && (Port == A.Port));
    }

    bool operator != (const IpAddress& A)
    {
        return !((IP == A.IP) && (Port == A.Port));
    }

    QString IP;
    unsigned int Port = 0;
};

typedef struct AddressBlock
{

    AddressBlock() {}
    AddressBlock(const AddressBlock& A)
    {
        From = A.From;
        To = A.To;
        Direction = A.Direction;
        BypassArray = A.BypassArray;
        PathCount = A.PathCount;
    }

    bool operator == (const AddressBlock& AnotherAddressBlock)
    {
        return From.IP == AnotherAddressBlock.From.IP &&
               From.Port == AnotherAddressBlock.From.Port &&
               To.IP == AnotherAddressBlock.To.IP &&
               To.Port == AnotherAddressBlock.To.Port &&
               Direction == AnotherAddressBlock.Direction &&
               BypassArray == AnotherAddressBlock.BypassArray &&
               PathCount == AnotherAddressBlock.PathCount;
    }

    IpAddress From;
    IpAddress To;
    BypassDirection Direction;
    QJsonArray BypassArray;
    unsigned int PathCount;

} AddressBlock;

class Serlize: QObject
{
    Q_OBJECT
public:

    inline static QJsonArray SerlizeIPAddress(QVector<IpAddress> Address)
    {
        QJsonArray ReturnAddress;

        for (IpAddress ServerAddress : Address)
        {
            QJsonObject InsertAddress;
            InsertAddress["ip"] = ServerAddress.IP;
            InsertAddress["port"] = static_cast<int>(ServerAddress.Port);

            ReturnAddress.append(InsertAddress);
        }

        return ReturnAddress;
    }

    inline static QVector<IpAddress> DeSerlize(QJsonArray Array)
    {
        QVector<IpAddress> ReturnValue;

        for (QJsonValue obj : Array)
        {
            IpAddress InsertAddress;
            InsertAddress.IP = obj["ip"].toString();
            InsertAddress.Port = obj["port"].toInt();

            ReturnValue.append(InsertAddress);
        }

        return ReturnValue;
    }
};

}

}


#endif // ADDRESSBLOCK_H
