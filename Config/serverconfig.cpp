#include "serverconfig.h"

Cuma::Config::ServerConfig::ServerConfig(QObject *parent) : QObject(parent)
{

}

Cuma::Config::ServerConfig::ServerConfig(QJsonObject obj)
{
    Config = obj;
}

Cuma::Config::ServerConfig::~ServerConfig()
{

}

int Cuma::Config::ServerConfig::GetThreadCount()
{
    if (Config.find("Thread") != Config.end())
    {
        return Config["Thread"].toInt();
    }

    return -1;
}
