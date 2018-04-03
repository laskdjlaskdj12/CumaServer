#include "qtjsonsocketlib_v3.h"
#define DEFAULTSRESOURCESIZE 6

QtJsonSocketLib_v3::QtJsonSocketLib_v3(QObject* parent ) : Qt_Json_Socket_Lib(),
    DefaultTimeout(3000),
    TimeoutActive(true),
    EmitEvent(false),
    ErrType(QtJsonSocketError::None)
{

}

QtJsonSocketLib_v3::QtJsonSocketLib_v3(bool TimeoutActive, bool EmitEvent): TimeoutActive(TimeoutActive),
    DefaultTimeout(3000),
    EmitEvent(EmitEvent),
    ErrType(QtJsonSocketError::None)
{

}

QtJsonSocketLib_v3::~QtJsonSocketLib_v3()
{
    if (Socket == nullptr)
    {
        Socket.clear();
        return;
    }

    if (Socket.isNull() == false)
    {
        disconnect_socket();
    }

    Socket.clear();
}

bool QtJsonSocketLib_v3::make_QTcpSocket()
{
    Socket = QSharedPointer<QTcpSocket>::create();

    if (EmitEvent == true)
    {
        QObject::connect(Socket.data(), &QIODevice::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
    }

    QObject::connect(Socket.data(), &QAbstractSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);

    return !Socket.isNull();
}

bool QtJsonSocketLib_v3::SwapSocket(QTcpSocket* sock)
{
    if (Socket.isNull() == false)
    {
        if (Socket->isOpen() == true)
        {
            Socket->close();
        }

        QObject::disconnect(Socket.data(), &QIODevice::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
        QObject::disconnect(Socket.data(), &QAbstractSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);

        Socket.clear();
    }

    Socket.reset(sock);

    if (EmitEvent == true)
    {
        QObject::connect(Socket.data(), &QIODevice::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
    }

    QObject::connect(Socket.data(), &QAbstractSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);
}

bool QtJsonSocketLib_v3::SetSocket(QTcpSocket* sock)
{
    if (Socket.isNull() == false)
    {
        return false;
    }

    if (Socket->isOpen() == false)
    {
        return false;
    }

    Socket.reset(sock);

    return true;
}

void QtJsonSocketLib_v3::set_socket(QTcpSocket* sock)
{
    if (Socket.isNull() == false)
    {
        if (Socket->isOpen() == true)
        {
            Socket->close();
        }

        QObject::disconnect(Socket.data(), &QIODevice::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
        QObject::disconnect(Socket.data(), &QAbstractSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);

        Socket.clear();
    }

    Socket.reset(sock);

    /*
     * 소켓의 데이터가 들어왔다는 시그널을 바인드
     * */

    if (EmitEvent == true)
    {
        QObject::connect(Socket.data(), &QTcpSocket::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
    }

    QObject::connect(Socket.data(), &QTcpSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);
}

QTcpSocket* QtJsonSocketLib_v3::get_socket()
{
    return Socket.data();
}

void QtJsonSocketLib_v3::set_defaulttimeout(int time)
{
    DefaultTimeout = time;
}

bool QtJsonSocketLib_v3::send_QByteArray(QByteArray arr)
{
    try
    {
        ClearErrorType();

        if (Socket->state() != QAbstractSocket::ConnectedState)
        {
            throw QtJsonSocketError::Connect;
        }

        qint64 ReesultSendByte = 0;
        qint64 ResultSendSource = 0;

        QByteArray SendByteSize = QString::number(arr.size()).toUtf8();
        SendByteSize.resize(DEFAULTSRESOURCESIZE);

        ReesultSendByte = Socket->write(QString::number(arr.size()).toUtf8(), DEFAULTSRESOURCESIZE);

        ResultSendSource = Socket->write(arr);

        if (ReesultSendByte == -1)
        {
            throw QtJsonSocketError::SendSocketSize;
        }

        if (ResultSendSource == -1)
        {
            throw QtJsonSocketError::SendSocketSource;
        }
    }

    catch (const QtJsonSocketError& e)
    {
        ErrorException(e);
        return false;
    }

    return true;
}

QByteArray QtJsonSocketLib_v3::recv_QByteArray()
{
    try
    {
        ClearErrorType();

        if (Socket->state() != QAbstractSocket::ConnectedState)
        {
            throw QtJsonSocketError::Connect;
        }

        QByteArray RecvSourceSize;
        QByteArray RecvSource;

        if (TimeoutActive == true && EmitEvent == false)
        {
            if (Socket->bytesAvailable() == 0 && Socket->waitForReadyRead(DefaultTimeout) == false)
            {
                throw QtJsonSocketError::RecvTimeout;
            }

            RecvSourceSize = Socket->read( DEFAULTSRESOURCESIZE);
        }

        else
        {
            RecvSourceSize = Socket->read( DEFAULTSRESOURCESIZE);
        }

        unsigned int RecvSourceSizeINt = RecvSource.length();

        if (RecvSourceSize.isEmpty() == true)
        {
            throw QtJsonSocketError::RecvSize;
        }

        unsigned int RecvBinarySize = 0;
        RecvBinarySize = static_cast<unsigned int>(RecvSourceSize.toInt());

        while (RecvBinarySize > RecvSource.size())
        {
            unsigned int ReadByteSize = RecvBinarySize - RecvSource.size();
            QByteArray TempRecvSource;
            TempRecvSource = Socket->read(ReadByteSize);
            RecvSource += TempRecvSource;
        }

        if (RecvSource.isEmpty())
        {
            throw QtJsonSocketError::RecvSource;
        }

        return RecvSource;
    }

    catch (const QtJsonSocketError& e)
    {
        ErrorException(e);

        QByteArray ReturnErrorQByte;
        return ReturnErrorQByte;
    }
}

bool QtJsonSocketLib_v3::send_Json(QJsonObject obj)
{
    QJsonDocument doc(obj);

    QByteArray SendJson = doc.toBinaryData();

    return send_QByteArray(SendJson);
}

QJsonObject QtJsonSocketLib_v3::RecvJson()
{

    QJsonObject ReturnObject;

    QJsonDocument doc = recv_Json();

    if (doc.isNull() == true)
    {
        if (ErrType != QtJsonSocketError::None)
        {
            return ReturnObject;
        }

        ErrorException(QtJsonSocketError::DecodeByteArrayToQJsonDocument);

        return ReturnObject;
    }

    ReturnObject = doc.object();

    return ReturnObject;
}

QJsonDocument QtJsonSocketLib_v3::recv_Json()
{
    QJsonDocument ReturnObject;

    QByteArray RecvJsonByteArray = recv_QByteArray();

    if (RecvJsonByteArray.isEmpty())
    {
        return ReturnObject;
    }

    ReturnObject = QJsonDocument::fromBinaryData(RecvJsonByteArray);

    return ReturnObject;
}

void QtJsonSocketLib_v3::connect_server(const QString& Ip, qint64 port)
{
    Socket->connectToHost(Ip, port);

    if (Socket->waitForConnected(DefaultTimeout) == false)
    {
        ErrorException(QtJsonSocketError::Connect);
    }
}

bool QtJsonSocketLib_v3::is_connect()
{
    if (Socket.isNull() == true)
    {
        return false;
    }

    bool Connected = (Socket->state() == QTcpSocket::ConnectedState);

    return Connected;
}

void QtJsonSocketLib_v3::disconnect_server()
{
    disconnect_socket();
}

void QtJsonSocketLib_v3::disconnect_socket()
{

    if (Socket.isNull() == false)
    {
        Socket->disconnectFromHost();
    }

    if (EmitEvent == true)
    {
        QObject::disconnect(Socket.data(), &QIODevice::readyRead, this, &QtJsonSocketLib_v3::OnRecvEventFromSocket);
    }

    QObject::disconnect(Socket.data(), &QAbstractSocket::disconnected, this, &QtJsonSocketLib_v3::OnDisconnectEventFromSocket);

}

void QtJsonSocketLib_v3::delete_QTcpSocket()
{
    if (Socket.isNull())
    {
        return;
    }

    if (is_connect())
    {
        disconnect_socket();
    }

    Socket.clear();
    Socket = nullptr;
}

QString QtJsonSocketLib_v3::GetErrorString()
{
    return ErrorString;
}

unsigned int QtJsonSocketLib_v3::ErrorType()
{
    return ErrType;
}

QString QtJsonSocketLib_v3::GetSocketErrorString()
{
    return SocketErrorString;
}

QAbstractSocket::SocketError QtJsonSocketLib_v3::GetSocketError()
{
    return SocketError;
}

bool QtJsonSocketLib_v3::IsTimeOut()
{
    if (Socket.isNull() == false && Socket->state() == QTcpSocket::SocketTimeoutError)
    {
        return true;
    }

    return false;
}

void QtJsonSocketLib_v3::ErrorException(const QtJsonSocketError& e)
{
    switch (e)
    {
        case QtJsonSocketError::Connect:
            ErrType = QtJsonSocketError::Connect;
            ErrorString = "[Error] : Server is not Connect" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::SendSocketSize:
            ErrType = QtJsonSocketError::SendSocketSize;
            ErrorString = "[Error] : Sned PayloadSize not Send" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::SendSocketSource:
            ErrType = QtJsonSocketError::SendSocketSource;
            ErrorString = "[Error] : Sned Source is not Send" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::SendTimeout:
            ErrType = QtJsonSocketError::SendTimeout;
            ErrorString = "[Error] : Send Timeout" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::RecvTimeout:
            ErrType = QtJsonSocketError::RecvTimeout;
            ErrorString = "[Error] : Recv Timeout " + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::RecvSource:
            ErrType = QtJsonSocketError::RecvSource;
            ErrorString = "[Error] : RecvSource is not Recv" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;

        case QtJsonSocketError::RecvSize:
            ErrType = QtJsonSocketError::RecvSize;
            ErrorString = "[Error] : RecvSize is not Recv" + ErrorString;
            DEBUGLOG(Socket->errorString());
            break;
    }
}

void QtJsonSocketLib_v3::OnRecvEventFromSocket()
{
    emit OnRecvEvent();
}

void QtJsonSocketLib_v3::OnDisconnectEventFromSocket()
{
    emit OnDisconnectEvent();
}

void QtJsonSocketLib_v3::ClearErrorType()
{
    ErrType = QtJsonSocketError::None;
    ErrorString.clear();
}
