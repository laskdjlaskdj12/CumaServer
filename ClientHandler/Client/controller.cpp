#include "controller.h"

ClientController::ClientController(QSharedPointer<QtJsonSocketLib_v3>& client,
                                   QSharedPointer<Cuma::NetworkConfig::ServerList>& ServerList,
                                   QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                                   QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                                   QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileStorage,
                                   QCryptographicHash::Algorithm Algorithm):
    Client(client),
    ServerList(ServerList),
    DbAddressPath(DbAddressPath),
    DbFileFragInfo(DbFileFragInfo),
    FileBlockStorage(FileStorage),
    Algorithm(Algorithm)
{

}

ClientController::~ClientController()
{

}

void ClientController::ReplyControl(Cuma::Protocol::CumaProtocolBlock RecvProtocol)
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

bool ClientController::SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block)
{
    return client->send_Json(Cuma::Protocol::Serlize::SerlizeProtocol(Block));

}

Cuma::Protocol::CumaProtocolBlock ClientController::RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client)
{
    QJsonObject recvObject = client->RecvJson();
    return Cuma::Protocol::Serlize::DeSerlizeProtocol(recvObject);
}

void ClientController::ConnectHandler(Cuma::Protocol::CumaProtocolBlock Block)
{
    QJsonObject connectResult;
    connectResult["connect"] = true;

    Block.Data = QJsonDocument(connectResult).toJson();

    SendBlock(Client, Block);
}
