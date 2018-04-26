#include <QObject>
#include <QTest>
#include <QString>
#include <QThread>
#include <QUrl>
#include <QSignalSpy>
#include <QVariant>
#include <QSharedPointer>

#include "server.h"
#include "Block/FileBlock/blockstruct.h"
#include "ServerList/serverlist.h"
#include "clientmoc.h"

class ServerTest : public QObject
{
    Q_OBJECT

public:
    ServerTest()
    {

    }

    ~ServerTest()
    {

    }

    QSharedPointer<Cuma::NetworkConfig::ServerList> makeSingleServerList()
    {
        QSharedPointer<Cuma::NetworkConfig::ServerList> ReturnServerList = QSharedPointer<Cuma::NetworkConfig::ServerList>::create("Test.txt");

        ReturnServerList->Add(QUrl("127.0.0.1"), 7071);

        return ReturnServerList;
    }

    void StartClientThread(QSharedPointer<TestClient> Client, QSharedPointer<QThread> Thread)
    {
        Client->moveToThread(Thread.data());
        connect(Thread.data(), &QThread::started, Client.data(), &TestClient::Start);
        connect(Thread.data(), &QThread::finished, Client.data(), &TestClient::Stop);
        connect(this, &ServerTest::SendProtocol, Client.data(), &TestClient::SendRequest);

        Thread->start();
    }

    void StartServerThread(QSharedPointer<Cuma::Server> Server, QSharedPointer<QThread> Thread)
    {
        Server->moveToThread(Thread.data());
        connect(Thread.data(), &QThread::started, Server.data(), &Cuma::Server::OnStart);
        connect(Thread.data(), &QThread::finished, Server.data(), &Cuma::Server::OnStop);

        Thread->start();
    }


    void StopServerAndClient(QSharedPointer<TestClient> Client, QSharedPointer<QThread> ClientThread, QSharedPointer<Cuma::Server> Server, QSharedPointer<QThread> ServerThread)
    {
        //클라이언트 쓰레드 종료
        ClientThread->quit();

        while (!ClientThread->wait())
        {
            QThread::msleep(10);
        }

        //서버 쓰레드를 종료
        ServerThread->quit();

        while (!ServerThread->wait())
        {
            QThread::msleep(10);
        }
    }

protected:
    Cuma::Protocol::CumaProtocolBlock makeConnectionBlock()
    {
        Cuma::Protocol::CumaProtocolBlock ReturnBlock;

        ReturnBlock.Address.To.IP = "127.0.0.1";
        ReturnBlock.Address.To.Port = 7071;

        ReturnBlock.ProtocolType = Cuma::Protocol::Connect;
        ReturnBlock.Address.Direction = Cuma::Address::Req;

        return ReturnBlock;
    }

    Cuma::Protocol::CumaProtocolBlock makeFileSaveRequest()
    {
        Cuma::FileBlock::FileBlock RequestSaveBlock;

        RequestSaveBlock.FileName = "Test.txt";
        RequestSaveBlock.FileSize = sizeof("Test.txt");
        RequestSaveBlock.FileSource = "This is Test Source";
        RequestSaveBlock.FileFragCount = 1;
        RequestSaveBlock.Index = 0;
        RequestSaveBlock.Version = 1;

        Cuma::Protocol::CumaProtocolBlock ReturnBlock;

        ReturnBlock.Address.To.IP = "127.0.0.1";
        ReturnBlock.Address.To.Port = 7071;

        ReturnBlock.ProtocolType = Cuma::Protocol::Spread;
        ReturnBlock.Address.Direction = Cuma::Address::Req;

        return ReturnBlock;
    }

protected:
    Cuma::FileBlock::FileBlock makeSpreadFileMoc()
    {
        Cuma::FileBlock::FileBlock RetFileBlock;
        RetFileBlock.FileFragCount = 1;
        RetFileBlock.FileName = "Test.txt";
        RetFileBlock.FileSize = QString("Hello World").size();
        RetFileBlock.FileSource = "Hello World";
        RetFileBlock.Index = 0;
        RetFileBlock.Version = 1.0;

        return RetFileBlock;
    }

    Cuma::Protocol::CumaProtocolBlock makeRequestSpread(Cuma::FileBlock::FileBlock Block)
    {
        Cuma::Protocol::CumaProtocolBlock ReturnBlock;

        ReturnBlock.Address.From.IP = "127.0.0.1";
        ReturnBlock.Address.From.Port = 7070;
        ReturnBlock.Address.To.IP = "127.0.0.1";
        ReturnBlock.Address.To.Port = 7071;
        ReturnBlock.ProtocolType = Cuma::Protocol::Spread;
        ReturnBlock.Address.Direction = Cuma::Address::Req;
        ReturnBlock.Data = QJsonDocument(Cuma::FileBlock::Serlize::FileBlockToJson(Block)).toJson();

        return ReturnBlock;
    }

    QSharedPointer<Cuma::Server> makeServer(QSharedPointer<Cuma::Server> Server)
    {
        QSharedPointer<Cuma::NetworkConfig::ServerList> TempServerlist = makeSingleServerList();

        //서버에서 사용할 DbAddress와 FileFragDir를 만듬
        TestDbAddressByFile = QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>::create();
        TestDbAddressFileFrag = QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>::create();
        TestFileBlockStorage = QSharedPointer<Cuma::FileBlockStorage::FileFragDir>::create();

        TestFileBlockStorage->SetFileSaveLocation(QDir::currentPath());

        Server = QSharedPointer<Cuma::Server>::create(7071,
                 TempServerlist,
                 TestDbAddressFileFrag,
                 TestDbAddressByFile,
                 TestFileBlockStorage);
        return Server;
    }

    Cuma::FileBlock::FileFragInfo makeSearchFileFragInfo()
    {
        Cuma::FileBlock::FileFragInfo ReturnFileInfo;

        ReturnFileInfo.FileName = "Test.txt";
        ReturnFileInfo.Index = 0;

        return ReturnFileInfo;
    }

    Cuma::Protocol::CumaProtocolBlock makeRequestSearch(Cuma::FileBlock::FileFragInfo info)
    {
        Cuma::Protocol::CumaProtocolBlock ReturnBlock;

        ReturnBlock.Address.From.IP = "127.0.0.1";
        ReturnBlock.Address.From.Port = 7070;
        ReturnBlock.Address.To.IP = "127.0.0.1";
        ReturnBlock.Address.To.Port = 7071;
        ReturnBlock.ProtocolType = Cuma::Protocol::Search;
        ReturnBlock.Address.Direction = Cuma::Address::Req;
        ReturnBlock.Data = QJsonDocument(Cuma::FileBlock::Serlize::FileInfoToJson(info)).toJson();

        return ReturnBlock;
    }

    Cuma::FileBlock::FileFragInfo makeDownloadFileInfo(Cuma::FileBlock::FileBlock FileBlock)
    {
        Cuma::FileBlock::FileFragInfo ReturnInfo;
        ReturnInfo.FileName = FileBlock.FileName;
        ReturnInfo.FilePid = QCryptographicHash::hash(FileBlock.FileSource, QCryptographicHash::Sha3_512);
        ReturnInfo.Index = 0;

        return ReturnInfo;
    }

    Cuma::Protocol::CumaProtocolBlock makeRequestDownload(Cuma::FileBlock::FileFragInfo info)
    {
        Cuma::Protocol::CumaProtocolBlock ReturnBlock;

        ReturnBlock.Address.From.IP = "127.0.0.1";
        ReturnBlock.Address.From.Port = 7070;
        ReturnBlock.Address.To.IP = "127.0.0.1";
        ReturnBlock.Address.To.Port = 7071;
        ReturnBlock.ProtocolType = Cuma::Protocol::Download;
        ReturnBlock.Address.Direction = Cuma::Address::Req;
        ReturnBlock.Data = QJsonDocument(Cuma::FileBlock::Serlize::FileInfoToJson(info)).toJson();

        return ReturnBlock;
    }

private:
    QSharedPointer<Cuma::DbAddress::DbAddressPathByFile> TestDbAddressByFile;
    QSharedPointer<Cuma::FileBlockStorage::FileFragDir> TestFileBlockStorage;
    QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo> TestDbAddressFileFrag;

signals:
    void SendProtocol(const QString ip, const unsigned int port, const Cuma::Protocol::CumaProtocolBlock Block);

private slots:
    //    void ServerConnect()
    //    {
    //        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

    //        //ServerList를 구성
    //        QSharedPointer<Cuma::NetworkConfig::ServerList> TempServerlist = makeSingleServerList();

    //        //서버에서 사용할 DbAddress와 FileFragDir를 만듬
    //        QSharedPointer<Cuma::DbAddress::DbAddressPathByFile> TestDbAddressByFile;
    //        QSharedPointer<Cuma::FileBlockStorage::FileFragDir> TestFileBlockStorage;

    //        //moc클라이언트를 실행
    //        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
    //        StartClientThread(Client, ClientThread);

    //        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

    //        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<Cuma::Server> Server = QSharedPointer<Cuma::Server>::create(7071,
    //                                              TempServerlist,
    //                                              TestDbAddressByFile,
    //                                              TestFileBlockStorage);

    //        StartServerThread(Server, ServerThread);

    //        //클라이언트의 Connection을 요청
    //        emit SendProtocol("127.0.0.1", 7071, makeConnectionBlock());

    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());
    //        StopServerClient(Client, ClientThread, Server, ServerThread);
    //    }

    //    void ServerSpread()
    //    {
    //        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

    //        //서버를 실행
    //        QSharedPointer<Cuma::Server> Server = makeServer(Server);
    //        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
    //        StartServerThread(Server, ServerThread);

    //        //moc클라이언트를 실행
    //        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
    //        StartClientThread(Client, ClientThread);

    //        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

    //        Cuma::FileBlock::FileBlock SpreadFileBlock = makeSpreadFileMoc();

    //        Cuma::Protocol::CumaProtocolBlock RequestSpread = makeRequestSpread(SpreadFileBlock);

    //        emit SendProtocol("127.0.0.1", 7071, RequestSpread);

    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());

    //        StopServerAndClient(Client, ClientThread, Server, ServerThread);
    //    }

    //    void ServerSpread()
    //    {
    //        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

    //        //서버를 실행
    //        QSharedPointer<Cuma::Server> Server = makeServer(Server);
    //        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
    //        StartServerThread(Server, ServerThread);

    //        //moc클라이언트를 실행
    //        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
    //        StartClientThread(Client, ClientThread);

    //        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

    //        Cuma::FileBlock::FileFragInfo SpreadFileBlock = makeSearchFileFragInfo();

    //        Cuma::Protocol::CumaProtocolBlock RequestSpread = makeSearchFileProtocol(SpreadFileBlock);

    //        emit SendProtocol("127.0.0.1", 7071, RequestSpread);

    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());

    //        StopServerAndClient(Client, ClientThread, Server, ServerThread);
    //    }

    //    void ServerSearch()
    //    {
    //        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

    //        //서버를 실행
    //        QSharedPointer<Cuma::Server> Server = makeServer(Server);
    //        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
    //        StartServerThread(Server, ServerThread);

    //        //moc클라이언트를 실행
    //        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
    //        StartClientThread(Client, ClientThread);

    //        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

    //        Cuma::FileBlock::FileFragInfo SpreadFileBlock = makeSearchFileFragInfo();

    //        Cuma::Protocol::CumaProtocolBlock RequestSpread = makeRequestSearch(SpreadFileBlock);

    //        emit SendProtocol("127.0.0.1", 7071, RequestSpread);

    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());

    //        StopServerAndClient(Client, ClientThread, Server, ServerThread);
    //    }

    //    void ServerSuccessSearch()
    //    {
    //        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

    //        //서버를 실행
    //        QSharedPointer<Cuma::Server> Server = makeServer(Server);
    //        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
    //        StartServerThread(Server, ServerThread);

    //        //moc클라이언트를 실행
    //        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
    //        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
    //        StartClientThread(Client, ClientThread);

    //        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

    //        //파일을 Spread로 전송
    //        Cuma::FileBlock::FileBlock SpreadFileBlock = makeSpreadFileMoc();

    //        Cuma::Protocol::CumaProtocolBlock RequestSpread = makeRequestSpread(SpreadFileBlock);

    //        emit SendProtocol("127.0.0.1", 7071, RequestSpread);

    //        //파일의 정상적으로 Spread가 됬는지 확인
    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        Cuma::FileBlock::FileFragInfo SearchFileBlock = makeSearchFileFragInfo();

    //        Cuma::Protocol::CumaProtocolBlock RequestSearch = makeRequestSearch(SearchFileBlock);

    //        emit SendProtocol("127.0.0.1", 7071, RequestSearch);

    //        QVERIFY (ClientRecvSignal.wait(5000) == true);

    //        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());

    //        StopServerAndClient(Client, ClientThread, Server, ServerThread);
    //    }

    void ServerSuccessDownload()
    {
        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

        //서버를 실행
        QSharedPointer<Cuma::Server> Server = makeServer(Server);
        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
        StartServerThread(Server, ServerThread);

        //moc클라이언트를 실행
        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
        StartClientThread(Client, ClientThread);

        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

        //파일을 Spread로 전송
        Cuma::FileBlock::FileBlock SpreadFileBlock = makeSpreadFileMoc();

        Cuma::Protocol::CumaProtocolBlock RequestSpread = makeRequestSpread(SpreadFileBlock);

        emit SendProtocol("127.0.0.1", 7071, RequestSpread);

        //파일의 정상적으로 Spread가 됬는지 확인
        QVERIFY (ClientRecvSignal.wait(5000) == true);

        Cuma::FileBlock::FileFragInfo SearchFileBlock = makeDownloadFileInfo(SpreadFileBlock);

        Cuma::Protocol::CumaProtocolBlock RequestDownload = makeRequestDownload(SearchFileBlock);

        emit SendProtocol("127.0.0.1", 7071, RequestDownload);

        QVERIFY (ClientRecvSignal.wait(5000) == true);

        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());

        StopServerAndClient(Client, ClientThread, Server, ServerThread);
    }

};

QTEST_MAIN(ServerTest)

