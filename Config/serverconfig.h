#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QObject>
#include <QJsonObject>

namespace Cuma
{

namespace Config
{

class ServerConfig : public QObject
{
    Q_OBJECT
public:
    ServerConfig(QObject *parent = nullptr);

    ServerConfig(QJsonObject obj);

    ~ServerConfig();

    int GetThreadCount();

protected:
    QJsonObject Config;

};

}

}

#endif // SERVERCONFIG_H
