#include "serverlist.h"

Cuma::NetworkConfig::ServerList::ServerList(QString ServerFile)
{
    File.setFileName(ServerFile);

    if (File.open(QFile::ReadWrite) == false)
    {
        ErrorStr = "File is not Open";
        Error = true;
    }

    QByteArray FileResource = File.readAll();

    if (FileResource.isEmpty())
    {
        Error = false;
        return;
    }

    DEBUGLOG ("FileResource is :" + FileResource );

    QJsonParseError ParseError;
    QJsonDocument AddressListResource = QJsonDocument::fromJson(FileResource, &ParseError);

    if (AddressListResource.isNull() )
    {
        qDebug() << "JsonParseError : " + ParseError.errorString();
        ErrorStr = "Parse is Error";
        Error = true;
    }

    AddressObject = AddressListResource.object();
}

Cuma::NetworkConfig::ServerList::~ServerList()
{
    if (File.isOpen())
    {
        if (File.flush() == false)
        {
            qDebug() << " Error of File Close : " + File.errorString();
        }

        File.close();
    }
}

bool Cuma::NetworkConfig::ServerList::Add(const QUrl ip, const unsigned int port)
{
    if (Error == true)
    {
        ErrorStr = "Initalize ServerList Error";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }

    if (ip.isEmpty())
    {
        ErrorStr = "Ip is Empty";
        return false;
    }

    QJsonObject Address;
    Address["ip"] = ip.toString();
    Address["port"] = static_cast<int>(port);

    QJsonArray Array = AddressObject["Address"].toArray();
    Array.append(Address);
    AddressObject["Address"] = Array;

    QJsonDocument WriteDocument(AddressObject);

    File.resize(0);
    File.write(WriteDocument.toJson());

    return true;
}

bool Cuma::NetworkConfig::ServerList::Add(const QJsonArray ServerChain)
{
    if (Error == true)
    {
        ErrorStr = "Initalize ServerList Error";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }

    if (ServerChain.isEmpty())
    {
        ErrorStr = "ServerChain is Empty";
        return false;
    }

    QJsonValue ServerChainObject = ServerChain;

    QJsonArray Array = AddressObject["Address"].toArray();
    Array.append(ServerChainObject);
    AddressObject["Address"] = Array;

    QJsonDocument WriteDocument(AddressObject);

    File.resize(0);
    File.write(WriteDocument.toJson());

    return true;
}

bool Cuma::NetworkConfig::ServerList::Remove(const QJsonArray ServerArray)
{
    if (Error == true)
    {
        ErrorStr = "File Init Fail";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }
    if (AddressObject.find("Address") == AddressObject.end())
    {
        ErrorStr = "Address 오브젝트가 없습니다.";
        return true;
    }

    QJsonArray AddressList = AddressObject["Address"].toArray();

    bool IsErase = false;

    for (unsigned int i = 0; i < AddressList.count(); i++)
    {
        //만약 서버 ArrayList이라면 해당 리스트가 맞는지 확인
        if (AddressList.at(i).isArray())
        {
            QJsonArray CompareBypassServer = AddressList.at(i).toArray();

            if (CompareBypassServer == ServerArray)
            {
                AddressList.removeAt(i);
                IsErase = true;
            }
        }
    }

    if (IsErase == false)
    {
        ErrorStr = "Can't find AddressList";
        return false;
    }

    AddressObject["Address"] = AddressList;

    QJsonDocument WriteDocument(AddressObject);

    File.resize(0);

    File.write(WriteDocument.toJson());

    return true;
}

bool Cuma::NetworkConfig::ServerList::Remove(const QUrl ip, const unsigned int port)
{
    if (Error == true)
    {
        ErrorStr = "File Init Fail";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }
    if (AddressObject.find("Address") == AddressObject.end())
    {
        ErrorStr = "Address 오브젝트가 없습니다.";
        return true;
    }

    QJsonArray AddressList = AddressObject["Address"].toArray();

    bool IsErase = false;

    for (unsigned int i = 0; i < AddressList.count(); i++)
    {
        QJsonObject Address = AddressList.at(i).toObject();

        if (Address["ip"].toString() == ip.toString() && Address["port"].toInt() == port)
        {
            AddressList.removeAt(i);
            IsErase = true;
            break;
        }
    }

    if (IsErase == false)
    {
        ErrorStr = "Can't find AddressList";
        return false;
    }

    AddressObject["Address"] = AddressList;

    QJsonDocument WriteDocument(AddressObject);

    File.resize(0);

    File.write(WriteDocument.toJson());

    return true;
}

bool Cuma::NetworkConfig::ServerList::RemoveAll()
{
    if (Error == true)
    {
        ErrorStr = "File Init Fail";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }

    AddressObject.remove("Address");
    File.resize(0);
    File.write(QJsonDocument(AddressObject).toJson());
    return true;
}

bool Cuma::NetworkConfig::ServerList::Exsist(const QJsonArray ServerChain)
{
    if (Error == true)
    {
        ErrorStr = "File Init Fail";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }

    if (AddressObject.find("Address") == AddressObject.end())
    {
        ErrorStr = "Address 오브젝트가 없습니다.";
        return false;
    }

    QJsonArray AddressList = AddressObject["Address"].toArray();
    for (auto a : AddressList)
    {
        if (a.isArray() == false)
        {
            continue;
        }

        QJsonArray IteratorBypassServerArray = a.toArray();

        if (IteratorBypassServerArray == ServerChain)
        {
            return true;
        }
    }

    return false;
}

bool Cuma::NetworkConfig::ServerList::Exsist(const QUrl ip, const unsigned int port)
{
    if (Error == true)
    {
        ErrorStr = "File Init Fail";
        return false;
    }

    if (File.isOpen() == false)
    {
        ErrorStr = "File is not Open";
        return false;
    }

    if (AddressObject.find("Address") == AddressObject.end())
    {
        ErrorStr = "Address 오브젝트가 없습니다.";
        return false;
    }

    QJsonArray AddressList = AddressObject["Address"].toArray();
    for (auto a : AddressList)
    {
        if (a.isArray())
        {
            continue;
        }

        if (a.toObject()["ip"].toString() == ip.toString() && a.toObject()["port"].toInt() == port)
        {
            return true;
        }
    }

    return false;
}

void Cuma::NetworkConfig::ServerList::Close()
{
    File.write(QJsonDocument(AddressObject).toJson());

    File.close();

    bool Error = true;
}

QVector<QString> Cuma::NetworkConfig::ServerList::GetAllServerList()
{
    QVector<QString> RetList;

    for (auto a : AddressObject["Address"].toArray())
    {
        QString InsertIt = a.toObject()["ip"].toString() + " : " + QString::number(a.toObject()["port"].toInt());
        RetList.push_back(InsertIt);
    }

    return RetList;
}

QJsonArray Cuma::NetworkConfig::ServerList::GetAllServerListToArray()
{
    return AddressObject["Address"].toArray();
}

QVector<Cuma::Address::IpAddress> Cuma::NetworkConfig::ServerList::GetAllServerIpAddressList()
{
    QVector<Cuma::Address::IpAddress> ServerList;

    QJsonArray Array = AddressObject["Address"].toArray();

    for (QJsonValue value : Array)
    {
        QString IP = value.toObject()["ip"].toString();
        unsigned int Port = static_cast<unsigned int>(value.toObject()["port"].toInt());

        Cuma::Address::IpAddress ipaddress;
        ipaddress.IP = IP;
        ipaddress.Port = Port;

        ServerList.append(ipaddress);
    }

    return ServerList;
}

QString Cuma::NetworkConfig::ServerList::ErrorString()
{
    return ErrorStr;
}
