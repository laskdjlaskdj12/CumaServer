#include "server.h"

Cuma::Server::Server(unsigned int Port, QSharedPointer<Cuma::NetworkConfig::ServerList> list):
    List(list),
    port(Port)
{
    ConnectRequestServer = QSharedPointer<QTcpServer>::create();
}

Cuma::Server::~Server()
{
    ConnectRequestServer->close();

    for (QSharedPointer<Cuma::ClientHandler> iter : RunningClientHandler)
    {
        iter->Stop();
    }
}

void Cuma::Server::OnConnect()
{
    QTcpSocket* Socket = ConnectRequestServer->nextPendingConnection();

    QSharedPointer<QtJsonSocketLib_v3> JsonSocket = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);

    JsonSocket->set_socket(Socket);

    QSharedPointer<Cuma::ClientHandler> Handler = QSharedPointer<Cuma::ClientHandler>::create(JsonSocket, List);

    QSharedPointer<QThread> Thread = QSharedPointer<QThread>::create();

    BindThreadClientHandler(Handler, Thread);

    Cuma::Address::IpAddress ConnectFromClient;
    ConnectFromClient.IP = Socket->peerAddress().toString();
    ConnectFromClient.Port = Socket->peerPort();

    QString StringIpAddress = ConnectFromClient.IP + ":" + QString::number(ConnectFromClient.Port);

    RunningClientHandler.insert(StringIpAddress, Handler);
    ClientHandlerThreadList.append(Thread);

    Thread->start();
}

void Cuma::Server::OnStop()
{
    ConnectRequestServer->close();

    for (QSharedPointer<QThread> HandlerThread : ClientHandlerThreadList)
    {
        HandlerThread->quit();

        HandlerThread->wait();

        return;
    }
}

void Cuma::Server::OnStart()
{
    DEBUGLOG("서버 스레드가 시작되었습니다.");

    connect(ConnectRequestServer.data(), &QTcpServer::newConnection, this, &Server::OnConnect);

    ConnectRequestServer->listen(QHostAddress::Any, port);

    DEBUGLOG("포트 : " + QString::number(port));
}

void Cuma::Server::BindThreadClientHandler(const QSharedPointer<Cuma::ClientHandler>& Handler, const QSharedPointer<QThread>& Thread)
{
    Handler->moveToThread(Thread.data());
    connect(Thread.data(), &QThread::started, Handler.data(), &ClientHandler::Start);
    connect(Thread.data(), &QThread::finished, Handler.data(), &ClientHandler::Stop);
}
