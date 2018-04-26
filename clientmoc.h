#ifndef CLIENTMOC_H
#define CLIENTMOC_H

#include <QObject>
#include <QSharedPointer>
#include <QDebug>

#include "JsonSocketLib/qtjsonsocketlib_v3.h"
#include "Protocol/Template/protocoltemplate.h"
#include "Protocol/Type/protocoltype.h"
#include "Protocol/protocol.h"
#include "Log/errorlog.h"
#include "Log/debuglog.h"

class TestClient: public QObject
{
    Q_OBJECT
public:
    TestClient()
    {

    }
    ~TestClient()
    {

    }

public slots:
    inline void Start()
    {
        DEBUGLOG("클라이언트가 실행했습니다.");
        Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);
    }

    inline void Stop()
    {
        if (Socket->is_connect())
        {
            Socket->disconnect_socket();
        }

        Socket.clear();
    }

    bool ConnectServer(const unsigned int port, const QString ip)
    {
        Socket->connect_server(ip, port);

        if (Socket->is_connect() == false)
        {
            throw ("Client : Socket Connect Fail");
        }
    }

    void SendBlock(const Cuma::Protocol::CumaProtocolBlock Block) throw (const QString)
    {
        QJsonObject SendJson = Cuma::Protocol::Serlize::SerlizeProtocol(Block);

        if (Socket->send_Json(SendJson) == false)
        {
            throw ("Client : SendJson is Fail");
        }
    }

    void RecvBypassByPathCount(const Cuma::Protocol::CumaProtocolBlock Block)
    {
        int recvCount = Block.Address.BypassArray.count() - 1;

        for (int i = 0; i < Block.Address.BypassArray.count() - 1; i++)
        {
            DEBUGLOG("Bypass [" + QString::number(i) + "] 번째 수신 중..");
            QJsonObject RecvJson = Socket->RecvJson();

            if (RecvJson.isEmpty())
            {
                throw ("Client : RecvJson is Fail");
            }

            ReplyBlock = Cuma::Protocol::Serlize::DeSerlizeProtocol(RecvJson);

            DEBUGLOG (QJsonDocument(RecvJson).toJson());

            if (recvCount)
            {
                DEBUGLOG (QJsonDocument::fromJson(ReplyBlock.Data).toJson());
            }
        }
    }

    inline void RecvNoBypass(const Cuma::Protocol::CumaProtocolBlock Block)
    {
        QJsonObject RecvJson = Socket->RecvJson();

        if (RecvJson.isEmpty())
        {
            throw ("RecvJson 수신 실패 ");
        }

        DEBUGLOG (QJsonDocument(RecvJson).toJson());

        Cuma::Protocol::CumaProtocolBlock RecvBlock = Cuma::Protocol::Serlize::DeSerlizeProtocol(RecvJson);

        DEBUGLOG (RecvBlock.Data);
    }

    inline void SendRequest(const QString ip, const unsigned int port, const Cuma::Protocol::CumaProtocolBlock Block)
    {
        try
        {
            if (!Socket->is_connect())
            {
                if (Socket.isNull())
                {
                    throw ("Client : 소켓이 NULL입니다.");
                }

                Socket->make_QTcpSocket();

                DEBUGLOG(ip + ":" + port + " 서버에 접속중..");

                ConnectServer(port, ip);

            }

            SendBlock(Block);

            DEBUGLOG("클라이언트에서 전송을 했습니다.");

            //프로토콜이 Bypass일경우
            if (Block.Address.BypassArray.count() > 0 )
            {
                RecvBypassByPathCount(Block);
            }

            else
            {
                RecvNoBypass(Block);
            }

            emit Complete(true);
        }

        catch (QString& string)
        {
            ERRLOG(string);
            emit Complete(false);
        }
    }

    inline Cuma::Protocol::CumaProtocolBlock getReplyFromServer()
    {
        return ReplyBlock;
    }
signals:
    void Complete(bool Success);


private:
    QSharedPointer<QtJsonSocketLib_v3> Socket;
    Cuma::Protocol::CumaProtocolBlock ReplyBlock;
};
#endif // CLIENTMOC_H
