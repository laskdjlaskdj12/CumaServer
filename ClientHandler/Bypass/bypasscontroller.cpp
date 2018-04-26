#include "bypasscontroller.h"

BypassController::BypassController(Cuma::Protocol::CumaProtocolBlock RecvProtocol,
                                   QSharedPointer<QtJsonSocketLib_v3> Client):
    RecvProtocol(RecvProtocol),
    Client(Client),
    IsBypassBrockerActive(false)
{

}

BypassController::~BypassController()
{

}

void BypassController::UpdateBypassInfo(Cuma::Protocol::CumaProtocolBlock& Block)
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

QSharedPointer<QtJsonSocketLib_v3> BypassController::ConnectNextBypassServer(Cuma::Address::IpAddress address)
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

void BypassController::SendBypassConnectResult(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    QJsonObject connectResult;
    connectResult["connect"] = true;

    Block.Data = QJsonDocument(connectResult).toJson();

    SendBlock(client, Block);
}

void BypassController::StartBypassSession(QSharedPointer<QtJsonSocketLib_v3> PreviewBypassServer, QSharedPointer<QtJsonSocketLib_v3> NextBypassServer)
{
    bypassHandler = QSharedPointer<BypassHandler>::create(PreviewBypassServer, NextBypassServer);

    connect(bypassHandler.data(), SIGNAL(OnDisconnectNextServer()), this, SLOT(OnDisconnectNext()));
    connect(bypassHandler.data(), SIGNAL(OnDisconnectPreviewServer()), this, SLOT(OnDisconnectPreview()));
}

bool BypassController::StartBypassBroker()
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
        return IsBypassBrockerActive;
    }

    DEBUGLOG("중계모드로 전환 완료 세션 시작 ");
    StartBypassSession(Client, NextBypassServer);

    DEBUGLOG("다음 바이패스 프로토콜 전송");
    if ( SendBlock(NextBypassServer, RecvProtocol) == false)
    {
        ERRLOG(NextBypassServer->GetErrorString());
        return IsBypassBrockerActive;
    }

    Cuma::Protocol::CumaProtocolBlock DebugRecvBlock = RecvBlock(NextBypassServer);

    IsBypassBrockerActive = true;

    DEBUGLOG("전 노드에게 다음 바이패스 연결 성공 전송");
    SendBypassConnectResult(Client, RecvProtocol);

    return IsBypassBrockerActive;
}

bool BypassController::Stop()
{
    if (IsBypassBrockerActive)
    {
        return bypassHandler->Stop();
    }

    return true;
}

Cuma::Protocol::CumaProtocolBlock BypassController::RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client)
{
    QJsonObject recvObject = client->RecvJson();
    return Cuma::Protocol::Serlize::DeSerlizeProtocol(recvObject);
}

bool BypassController::SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    return client->send_Json(Cuma::Protocol::Serlize::SerlizeProtocol(Block));
}

void BypassController::OnDisconnectPreview()
{
    DEBUGLOG("이전 서버에서 접속 해제");
    emit DisconnectPreviewServer(true);
}

void BypassController::OnDisconnectNext()
{
    DEBUGLOG("다음 서버에서 접속 해제");
    emit DisconnectNextServer(true);
}

Cuma::Address::IpAddress BypassController::GetNextBypassAddress(const Cuma::Protocol::CumaProtocolBlock& Block)
{
    Cuma::Address::IpAddress NextBypassAddress;

    QJsonValue JsonNextAddress = Block.Address.BypassArray.at(Block.Address.PathCount + 1);

    NextBypassAddress.IP = JsonNextAddress.toObject()["ip"].toString();
    NextBypassAddress.Port = JsonNextAddress.toObject()["port"].toInt();

    return NextBypassAddress;
}
