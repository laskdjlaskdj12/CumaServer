#ifndef BYPASSHANDLER_H
#define BYPASSHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QSharedPointer>

#include "Protocol/protocol.h"
#include "Log/debuglog.h"
#include "Block/FileBlock/blockstruct.h"
#include "Block/AddressBlock/addressblock.h"
#include "JsonSocketLib//qtjsonsocketlib_v3.h"

class BypassHandler : public QObject
{
    Q_OBJECT
public:
    explicit BypassHandler (QSharedPointer<QtJsonSocketLib_v3> previewServer, QSharedPointer<QtJsonSocketLib_v3> nextServer);

    ~BypassHandler();

protected:
    bool SendBlock(QSharedPointer<QtJsonSocketLib_v3>& client, Cuma::Protocol::CumaProtocolBlock Block);

    Cuma::Protocol::CumaProtocolBlock RecvBlock(QSharedPointer<QtJsonSocketLib_v3>& client);

signals:
    void OnDisconnectPreviewServer();

    void OnDisconnectNextServer();

public slots:
    bool Stop();

    //Bypass 세션이 연결이 됬을경우
    virtual void OnPreviewBypassSessionRecv();

    virtual void OnPreviewBypassSessionDisconnect();

    virtual void OnNextBypassSessionRecv();

    virtual void OnNextBypassSessionDisconnect();

private:
    QSharedPointer<QtJsonSocketLib_v3> PreviewServer;
    QSharedPointer<QtJsonSocketLib_v3> NextServer;
};

#endif // BYPASSHANDLER_H
