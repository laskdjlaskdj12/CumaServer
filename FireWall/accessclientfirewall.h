#ifndef ACCESSCLIENTFIREWALL_H
#define ACCESSCLIENTFIREWALL_H

#include <QObject>

#include <QSharedPointer>

#include <QVariant>

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>

namespace Cuma
{

namespace NetworkConfig
{

class AccessClientFirewall : public QObject
{
    Q_OBJECT
public:
    explicit AccessClientFirewall(QObject* parent = nullptr);

    ~AccessClientFirewall();

    bool Add(QString ip, unsigned int port);

    bool Remove(QString ip, unsigned int port);

    bool Search(QString ip);

    bool isOpen();

    QString GetErrorMessage();

protected:

    QSqlDatabase AddressListDB;

    bool DBOpen;

    QString ErrorStr;
};

}

}

#endif // ACCESSCLIENTFIREWALL_H
