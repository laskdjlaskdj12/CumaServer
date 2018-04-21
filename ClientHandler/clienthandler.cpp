#include "clienthandler.h"

Cuma::ClientHandler::ClientHandler(QSharedPointer<QtJsonSocketLib_v3> client,
                                   QSharedPointer<Cuma::NetworkConfig::ServerList> ServerList,
                                   QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                                   QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                                   QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage,
                                   QCryptographicHash::Algorithm Algorithm):
    Client(client),
    ServerList(ServerList),
    IsBypassBrockerActive(false),
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

void Cuma::ClientHandler::ReplyControl(Cuma::Protocol::CumaProtocolBlock RecvProtocol)
{
    switch (RecvProtocol.ProtocolType)
    {
        case Cuma::Protocol::Type::Connect:
        {
            DEBUGLOG("Connect 프로토콜");
            ConnectHandler(RecvProtocol);
        }
        break;

        case Cuma::Protocol::Type::Download:
        {
            DEBUGLOG("Download 프로토콜");

            Cuma::Protocol::CumaProtocolBlock ReplyDownloadBlock;

            DownloadHandler Download(DbFileFragInfo,
                                     FileBlockStorage,
                                     RecvProtocol);

            if (Download.isSuccess)
            {
                QByteArray FilefragSource = Download.FileResource;

                Cuma::FileBlock::FileFragInfo info = Cuma::FileBlock::Serlize::JsonToFileInfo(QJsonDocument::fromJson(RecvProtocol.Data).object());
                ReplyDownloadBlock = DownloadHandler::MakeReplySuccess(RecvProtocol, FilefragSource, info);
            }

            else
            {
                DEBUGLOG("파일 로드 실패");

                if (Download.isFileNotExsist)
                {
                    DEBUGLOG("파일이 존재하지 않습니다.");
                }
                else if (Download.isReadFileError)
                {
                    DEBUGLOG("파일을 읽는데 실패했습니다.");
                }
                else if (Download.isFileParsingError)
                {
                    DEBUGLOG("파일 파싱을 하지 못했습니다.");
                }
                else if (Download.isReadFileError)
                {
                    DEBUGLOG("파일 리소스 읽기에 실패했습니다.");
                }

                ReplyDownloadBlock = DownloadHandler::MakeReplyFail(RecvProtocol);
            }

        }
        break;

        case Cuma::Protocol::Type::Spread:
        {
            DEBUGLOG("Spread 프로토콜");

            Cuma::Protocol::CumaProtocolBlock ReplySpreadBlock;

            SpreadHandler Handler(DbFileFragInfo,
                                  DbAddressPath,
                                  Algorithm,
                                  FileBlockStorage,
                                  RecvProtocol);

            if (Handler.IsSuccess())
            {
                Cuma::FileBlock::FileBlock ReadyToSpreadFileBlock = Handler.GetFileBlockToSpread();

                ReplySpreadBlock = SpreadHandler::MakeReplySuccess(RecvProtocol);
            }

            else
            {
                DEBUGLOG("파일 저장 실패");

                ReplySpreadBlock = SpreadHandler::MakeReplyFail(RecvProtocol, "Fail Save FileBlock");
            }

            //클라이언트들에게 전송
            SendBlock(Client, ReplySpreadBlock);
        }
        break;

        case Cuma::Protocol::Type::Search:
        {
            DEBUGLOG("Search 프로토콜");

            Cuma::Protocol::CumaProtocolBlock ReplySearchBlock;

            SearchHandler Handler(DbFileFragInfo,
                                  RecvProtocol);

            if (Handler.isFind())
            {
                ReplySearchBlock = SearchHandler::MakeReplySuccess(RecvProtocol);
            }
            else
            {
                DEBUGLOG("파일 찾기 실패");

                ReplySearchBlock = SearchHandler::MakeReplyFail(RecvProtocol);
            }

            //클라이언트에게 전송
            SendBlock(Client, ReplySearchBlock);
        }
        break;

        case Cuma::Protocol::Type::Ping:
        {

        } break;

        case Cuma::Protocol::Type::Disconnect:
        {

        } break;
    }
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

    //바이패스가 아닌 Reply하는 클래스
    else
    {
        DEBUGLOG("바이패스가 아닌 프로토콜을 수신 ");
        ReplyControl(RecvProtocol);
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

