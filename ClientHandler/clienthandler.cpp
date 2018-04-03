#include "clienthandler.h"

Cuma::ClientHandler::ClientHandler(QSharedPointer<QtJsonSocketLib_v3> client,
                                   QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList):
    Client(client),
    ServerList(ServerList),
    IsBypassBrockerActive(false)
{

}

Cuma::ClientHandler::~ClientHandler()
{

}

void Cuma::ClientHandler::Start()
{
    connect(Client.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnRecv()));
}

void Cuma::ClientHandler::Stop()
{
    if (IsBypassBrockerActive)
    {
        while (!bypassHandler->Stop())
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
        DEBUGLOG("다음 바이패스 서버에 연결하는 준비중..");
        UpdateBypassInfo(RecvProtocol);

        Cuma::Address::IpAddress NextServerAddress = GetNextBypassAddress(RecvProtocol);

        DEBUGLOG("다음 바이패스 주소 : " + NextServerAddress.IP + ":" + QString::number(NextServerAddress.Port));

        DEBUGLOG("다음 바이패스 서버에 연결 요청..");
        QSharedPointer<QtJsonSocketLib_v3> NextBypassServer = ConnectNextBypassServer(NextServerAddress);

        if (NextBypassServer.isNull())
        {
            ERRLOG("다음 바이패스에 연결 실패");
            return;
        }

        SendBlock(NextBypassServer, RecvProtocol);

        StartBypassSession(Client, NextBypassServer);

        IsBypassBrockerActive = true;

        SendConnectResult(Client, RecvProtocol);
    }

    //Reply하는 클래스
    else
    {
        ConnectHandler(RecvProtocol);

        Client->disconnect_socket();

        emit DisconnectClient(true);
    }
}

void Cuma::ClientHandler::OnDisconnectPreview()
{
    emit DisconnectPreviewServer(true);
}

void Cuma::ClientHandler::OnDisconnectNext()
{
    emit DisconnectNextServer(true);
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

void Cuma::ClientHandler::UpdateBypassInfo(Cuma::Protocol::CumaProtocolBlock& Block)
{
    if (Block.Address.Direction == Cuma::Address::Req)
    {
        Block.Address.PathCount += 1;
    }

    else
    {
        Block.Address.PathCount -= 1;
    }
}

QSharedPointer<QtJsonSocketLib_v3> Cuma::ClientHandler::ConnectNextBypassServer(Cuma::Address::IpAddress address)
{
    QSharedPointer<QtJsonSocketLib_v3> nextBypassServer = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);
    nextBypassServer->make_QTcpSocket();

    nextBypassServer->connect_server(address.IP, address.Port);

    if (!nextBypassServer->is_connect())
    {
        nextBypassServer.reset(nullptr);
        return nullptr;
    }

    return nextBypassServer;
}

void Cuma::ClientHandler::SendConnectResult(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    QJsonObject connectResult;
    connectResult["connect"] = true;

    Block.Data = QJsonDocument(connectResult).toJson();

    SendBlock(client, Block);
}

Cuma::Address::IpAddress Cuma::ClientHandler::GetNextBypassAddress(const Cuma::Protocol::CumaProtocolBlock& Block)
{
    Cuma::Address::IpAddress NextBypassAddress;

    QJsonValue JsonNextAddress = Block.Address.BypassArray.at(Block.Address.PathCount + 1);

    NextBypassAddress.IP = JsonNextAddress.toObject()["ip"].toString();
    NextBypassAddress.Port = JsonNextAddress.toObject()["port"].toInt();

    return NextBypassAddress;
}

void Cuma::ClientHandler::StartBypassSession(QSharedPointer<QtJsonSocketLib_v3> PreviewBypassServer, QSharedPointer<QtJsonSocketLib_v3> NextBypassServer)
{
    bypassHandler = QSharedPointer<BypassHandler>::create(PreviewBypassServer, NextBypassServer);

    connect(bypassHandler.data(), SIGNAL(OnDisconnectNextServer()), this, SLOT(OnDisconnectNext()));
    connect(bypassHandler.data(), SIGNAL(OnDisconnectPreviewServer()), this, SLOT(OnDisconnectPreview()));

    disconnect(Client.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnRecv()));
}

void Cuma::ClientHandler::ConnectHandler(Cuma::Protocol::CumaProtocolBlock Block)
{
    QJsonObject connectResult;
    connectResult["connect"] = true;

    Block.Data = QJsonDocument(connectResult).toJson();

    SendBlock(Client, Block);
}

bool Cuma::ClientHandler::SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    return client->send_Json(Cuma::Protocol::Serlize::SerlizeProtocol(Block));
}


Cuma::Protocol::CumaProtocolBlock Cuma::ClientHandler::RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client)
{
    QJsonObject recvObject = client->RecvJson();
    return Cuma::Protocol::Serlize::DeSerlizeProtocol(recvObject);
}

