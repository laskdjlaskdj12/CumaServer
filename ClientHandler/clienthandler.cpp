#include "clienthandler.h"

Cuma::ClientHandler::ClientHandler(QSharedPointer<QtJsonSocketLib_v3> client,
                                   QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList,
                                   QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                                   QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                                   QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage,
                                   QCryptographicHash::Algorithm Algorithm):
    Client(client),
    ServerList(ServerList),
    DbFileFragInfo(DbFileFragInfo),
    DbAddressPath(DbAddressPath),
    Algorithm(Algorithm),
    FileBlockStorage(FileStorage)
{

}

Cuma::ClientHandler::~ClientHandler()
{

}

void Cuma::ClientHandler::ReplyFailControl(const Cuma::Protocol::CumaProtocolBlock& RecvBlock, const QString& str)
{
    Cuma::Protocol::CumaProtocolBlock ErrorProtocolBlock;
    ErrorProtocolBlock = RecvBlock;
    ErrorProtocolBlock.Address.To = RecvBlock.Address.From;
    ErrorProtocolBlock.Address.From = RecvBlock.Address.To;

    QJsonObject ErrorData;
    ErrorData["Error"] = str;
    ErrorProtocolBlock.Data = QJsonDocument(ErrorData).toJson();

    SendBlock(Client, ErrorProtocolBlock);
}

Cuma::Protocol::CumaProtocolBlock Cuma::ClientHandler::RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client)
{
    QJsonObject recvObject = client->RecvJson();
    return Cuma::Protocol::Serlize::DeSerlizeProtocol(recvObject);
}

bool Cuma::ClientHandler::SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    return client->send_Json(Cuma::Protocol::Serlize::SerlizeProtocol(Block));
}

void Cuma::ClientHandler::Start()
{
    connect(Client.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnRecv()));
}

void Cuma::ClientHandler::Stop()
{
    if (ClientRequestBypass != nullptr)
    {
        while ( ClientRequestBypass->Stop())
        {
            QThread::msleep(100);
        }

        return;
    }

    else
    {
        disconnect(Client.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnRecv()));
    }
}


void Cuma::ClientHandler::OnRecv()
{
    DEBUGLOG("프로토콜 수신");
    Cuma::Protocol::CumaProtocolBlock RecvProtocol = RecvBlock(Client);

    //바이패스 연결을 원했을시에 바이패스 연결
    if (IsRequestBypass(RecvProtocol))
    {
        DEBUGLOG("바이패스 프로토콜 수신 ");
        BypassController Bypass(RecvProtocol,
                                Client);

        Bypass.StartBypassBroker();
    }

    //바이패스가 아닌 Reply하는 클래스
    else
    {
        DEBUGLOG("바이패스가 아닌 프로토콜을 수신 ");
        ClientController Controller(Client,
                                    ServerList,
                                    DbAddressPath,
                                    DbFileFragInfo,
                                    FileBlockStorage,
                                    Algorithm);

        Controller.ReplyControl(RecvProtocol);
    }
}

bool Cuma::ClientHandler::IsRequestBypass(const Cuma::Protocol::CumaProtocolBlock& Block)
{
    if (Block.Address.BypassArray.count() < 1)
    {
        return false;
    }

    if (Block.Address.Direction == Cuma::Address::Req)
    {
        if (Block.Address.PathCount + 1 >= Block.Address.BypassArray.count() - 1)
        {
            return false;
        }
    }

    else
    {
        if (Block.Address.PathCount < 1)
        {
            return false;
        }
    }

    return true;

}
