#ifndef QTJSONSOCKETLIB_V3_H
#define QTJSONSOCKETLIB_V3_H

#include <QObject>
#include <QTcpSocket>
#include <QMetaMethod>
#include <QSharedPointer>
#include <QDebug>
#include <QByteArray>

#include <QJsonObject>
#include <QJsonDocument>
#include "JsonSocketLib/qt_json_socket_lib.h"
#include "Log/debuglog.h"

enum QtJsonSocketError
{
    None,
    Connect,
    SendSocketSize,
    SendSocketSource,
    SendTimeout,
    RecvTimeout,
    RecvSource,
    RecvSize,
    DecodeByteArrayToQJsonDocument,

};

class QtJsonSocketLib_v3 : public Qt_Json_Socket_Lib
{
    Q_OBJECT
public:
    QtJsonSocketLib_v3(QObject* parent = nullptr);

    QtJsonSocketLib_v3(bool TimeoutActive, bool EmitEvent = false);

    ~QtJsonSocketLib_v3();

    virtual bool make_QTcpSocket() override;

    virtual bool SwapSocket(QTcpSocket* sock);
    virtual bool SetSocket(QTcpSocket* sock);
    virtual void set_socket(QTcpSocket* sock) override;

    virtual QTcpSocket* get_socket() ;

    virtual void set_defaulttimeout(int time);

    virtual bool send_QByteArray(QByteArray arr);
    virtual QByteArray recv_QByteArray();
    virtual bool send_Json(QJsonObject obj);
    virtual QJsonObject RecvJson();
    virtual QJsonDocument recv_Json();

    virtual void connect_server(const QString& Ip, qint64 port);
    virtual bool is_connect();

    virtual void disconnect_server();
    virtual void disconnect_socket();
    virtual void delete_QTcpSocket();

    virtual QString GetErrorString();
    virtual unsigned int ErrorType();

    virtual QString GetSocketErrorString();
    virtual QAbstractSocket::SocketError GetSocketError();

    virtual bool IsTimeOut();
    virtual void ClearErrorType();

protected:
    void ErrorException(const QtJsonSocketError& e);

signals:
    void OnRecvEvent();

    void OnDisconnectEvent();

public slots:
    void OnRecvEventFromSocket();

    void OnDisconnectEventFromSocket();

protected:
    QSharedPointer<QTcpSocket> Socket;

    bool TimeoutActive;

    bool EmitEvent;

    unsigned int DefaultTimeout;

    QtJsonSocketError ErrType;

    QString ErrorString;

    QAbstractSocket::SocketError SocketError;

    QString SocketErrorString;

};

#endif // QTJSONSOCKETLIB_V3_H
