#include "bypasshandler.h"

BypassHandler::BypassHandler(QSharedPointer<QtJsonSocketLib_v3> previewServer,
                             QSharedPointer<QtJsonSocketLib_v3> nextServer):
    PreviewServer(previewServer),
    NextServer(nextServer)
{
    connect(NextServer.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnNextBypassSessionRecv()));
    connect(NextServer.data(), SIGNAL(OnDisconnectEvent()), this, SLOT(OnNextBypassSessionDisconnect()));
    connect(PreviewServer.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnPreviewBypassSessionRecv()));
    connect(PreviewServer.data(), SIGNAL(OnDisconnectEvent()), this, SLOT(OnPreviewBypassSessionDisconnect()));
}

BypassHandler::~BypassHandler()
{
    if (PreviewServer->is_connect())
    {
        PreviewServer->disconnect_socket();
    }

    if (NextServer->is_connect())
    {
        NextServer->disconnect_server();
    }
}

bool BypassHandler::SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    return client->send_Json(Cuma::Protocol::Serlize::SerlizeProtocol(Block));
}

Cuma::Protocol::CumaProtocolBlock BypassHandler::RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client)
{
    Cuma::Protocol::CumaProtocolBlock ReturnBlock;
    QJsonObject RecvJson = client->RecvJson();
    ReturnBlock = Cuma::Protocol::Serlize::DeSerlizeProtocol(RecvJson);

    return ReturnBlock;
}

bool BypassHandler::Stop()
{
    if (PreviewServer->is_connect())
    {
        PreviewServer->disconnect_socket();
    }

    if (NextServer->is_connect())
    {
        NextServer->disconnect_server();
    }

    disconnect(NextServer.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnNextBypassSessionRecv()));
    disconnect(NextServer.data(), SIGNAL(OnDisconnectEvent()), this, SLOT(OnNextBypassSessionDisconnect()));
    disconnect(PreviewServer.data(), SIGNAL(OnRecvEvent()), this, SLOT(OnPreviewBypassSessionRecv()));
    disconnect(PreviewServer.data(), SIGNAL(OnDisconnectEvent()), this, SLOT(OnPreviewBypassSessionDisconnect()));

    return true;
}

void BypassHandler::OnPreviewBypassSessionRecv()
{
    Cuma::Protocol::CumaProtocolBlock RecvBlockInfo = RecvBlock(PreviewServer);
    SendBlock(NextServer, RecvBlockInfo);
}

void BypassHandler::OnPreviewBypassSessionDisconnect()
{
    NextServer->disconnect_socket();
    PreviewServer->disconnect_socket();

    emit OnDisconnectPreviewServer();
}

void BypassHandler::OnNextBypassSessionRecv()
{
    Cuma::Protocol::CumaProtocolBlock RecvBlockInfo = RecvBlock(NextServer);
    SendBlock(PreviewServer, RecvBlockInfo);
}

void BypassHandler::OnNextBypassSessionDisconnect()
{
    NextServer->disconnect_socket();
    PreviewServer->disconnect_socket();

    emit OnDisconnectNextServer();

}
