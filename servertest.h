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

    void makeFileSaveRequest()
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
    void ServerConnect()
    {
        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

        //ServerList를 구성
        QSharedPointer<Cuma::NetworkConfig::ServerList> TempServerlist = makeSingleServerList();

        //moc클라이언트를 실행
        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
        StartClientThread(Client, ClientThread);

        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
        QSharedPointer<Cuma::Server> Server = QSharedPointer<Cuma::Server>::create(7071, makeSingleServerList());
        StartServerThread(Server, ServerThread);

        //클라이언트의 Connection을 요청
        emit SendProtocol("127.0.0.1", 7071, makeConnectionBlock());

        QVERIFY (ClientRecvSignal.wait(5000) == true);

        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());
        StopServerClient(Client, ClientThread, Server, ServerThread);
    }

    void ServerSpread()
    {
        qRegisterMetaType<Cuma::Protocol::CumaProtocolBlock>("Cuma::Protocol::CumaProtocolBlock");

        //ServerList를 구성
        QSharedPointer<Cuma::NetworkConfig::ServerList> TempServerlist = makeSingleServerList();

        //moc클라이언트를 실행
        QSharedPointer<QThread> ClientThread = QSharedPointer<QThread>::create();
        QSharedPointer<TestClient> Client = QSharedPointer<TestClient>::create();
        StartClientThread(Client, ClientThread);

        QSignalSpy ClientRecvSignal(Client.data(), &TestClient::Complete);

        QSharedPointer<QThread> ServerThread = QSharedPointer<QThread>::create();
        QSharedPointer<Cuma::Server> Server = QSharedPointer<Cuma::Server>::create(7071, makeSingleServerList());
        StartServerThread(Server, ServerThread);

        //클라이언트의 Connection을 요청
        emit SendProtocol("127.0.0.1", 7071, makeFileSaveRequest());

        QVERIFY (ClientRecvSignal.wait(5000) == true);

        QVERIFY (ClientRecvSignal.takeFirst().at(0).toBool());
        StopServerClient(Client, ClientThread, Server, ServerThread);
    }
};
