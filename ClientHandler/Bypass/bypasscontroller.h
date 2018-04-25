#ifndef BYPASSCONTROLLER_H
#define BYPASSCONTROLLER_H

#include <QObject>
#include <QSharedPointer>

#include "Log/debuglog.h"
#include "Log/errorlog.h"
#include "Protocol/protocol.h"
#include "JsonSocketLib/qtjsonsocketlib_v3.h"
#include "BypassHandler/bypasshandler.h"

class BypassController : public QObject
{
    Q_OBJECT
public:
    BypassController(Cuma::Protocol::CumaProtocolBlock RecvProtocol,
                     QSharedPointer<QtJsonSocketLib_v3>& Client);

    ~BypassController();

    void UpdateBypassInfo(Cuma::Protocol::CumaProtocolBlock& Block);

    QSharedPointer<QtJsonSocketLib_v3> ConnectNextBypassServer(Cuma::Address::IpAddress address);

    void SendBypassConnectResult(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

    void StartBypassSession(QSharedPointer<QtJsonSocketLib_v3> PreviewBypassServer, QSharedPointer<QtJsonSocketLib_v3> NextBypassServer);

    bool Stop();
public:
    Cuma::Protocol::CumaProtocolBlock RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client);

    bool SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

protected:
    Cuma::Address::IpAddress GetNextBypassAddress(const Cuma::Protocol::CumaProtocolBlock& Block);

    //BypassHandler에서 상태이상이 있을때 Preview Server로 전송
public slots:
    void OnDisconnectPreview();

    void OnDisconnectNext();

signals:
    void DisconnectPreviewServer(bool Success);

    void DisconnectNextServer(bool Success);

private:
    QSharedPointer<BypassHandler> bypassHandler;

    bool IsBypassBrockerActive;

};

#endif // BYPASSCONTROLLER_H
