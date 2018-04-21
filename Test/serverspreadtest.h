#ifndef SERVERSPREADTEST_H
#define SERVERSPREADTEST_H


#include <QObject>
#include <QTest>
#include <QString>
#include <QThread>
#include <QUrl>
#include <QSignalSpy>
#include <QVariant>
#include <QSharedPointer>

#include "../server.h"
#include "../Block/FileBlock/blockstruct.h"
#include "../ServerList/serverlist.h"
#include "../clientmoc.h"

class ServerSpreadTest : public QObject
{
    Q_OBJECT

public:
    ServerSpreadTest()
    {

    }

    ~ServerSpreadTest()
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
        connect(this, &ServerSpreadTest::SendProtocol, Client.data(), &TestClient::SendRequest);

        Thread->start();
    }

    void StartServerThread(QSharedPointer<Cuma::Server> Server, QSharedPointer<QThread> Thread)
    {
        Server->moveToThread(Thread.data());
        connect(Thread.data(), &QThread::started, Server.data(), &Cuma::Server::OnStart);
        connect(Thread.data(), &QThread::finished, Server.data(), &Cuma::Server::OnStop);

        Thread->start();
    }


    void StopServerClient(QSharedPointer<TestClient> Client, QSharedPointer<QThread> ClientThread, QSharedPointer<Cuma::Server> Server, QSharedPointer<QThread> ServerThread)
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

    Cuma::Protocol::CumaProtocolBlock makeFileSpreadRequest()
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
signals:
    void SendProtocol(const QString ip, const unsigned int port, const Cuma::Protocol::CumaProtocolBlock Block);

private slots:
    void ServerSpread()
    {
        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

        //ServerList를 구성
        QSharedPointer<Cuma::NetworkConfig::ServerList> TempServerlist = makeSingleServerList();

        //moc클라이언트를 실행
        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
        StartClientThread(Client, ClientThread);

        //클라이언트에서 수신 체크할 시그널
        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

        //파일 블록이 어디에서 다운로드받은 Db를 구성
        QSharedPointer<Cuma::DbAddress::DbAddressPathByFile> DbAddress = QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>::create();
        QVERIFY2(DbAddress->isError(), DbAddress->GetErrorString().toStdString().c_str());

        //FileBlock을 저장할 스토리지
        QSharedPointer<Cuma::FileBlockStorage::FileFragDir> FileBlockStorage = QSharedPointer<Cuma::FileBlockStorage::FileFragDir>::create();

        QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo> DbFileFragInfo = QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>::create();

        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
        QSharedPointer<Cuma::Server> Server = QSharedPointer<Cuma::Server>::create(7071,
                                              TempServerlist,
                                              DbFileFragInfo,
                                              DbAddress,
                                              FileBlockStorage);
        StartServerThread(Server, ServerThread);

        //클라이언트에게 Spread를 요청
        emit SendProtocol("127.0.0.1", 7071, makeFileSpreadRequest());

        QVERIFY (ClientRecvSignal.wait(5000) == true);

        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());
        StopServerClient(Client, ClientThread, Server, ServerThread);
    }

};

#endif // SERVERSPREADTEST_H
