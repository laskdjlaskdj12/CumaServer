#ifndef DBADDRESSPATHBYFILE_H
#define DBADDRESSPATHBYFILE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSharedPointer>
#include <QVector>
#include <QVariant>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDebug>

#include "Block/AddressBlock/addressblock.h"
#include "Block/FileBlock/blockstruct.h"

namespace Cuma
{

namespace Address
{

class DbAddressPathByFile : public QObject
{
    Q_OBJECT
public:
    DbAddressPathByFile (QSqlDatabase DB);

    DbAddressPathByFile ();

    ~DbAddressPathByFile();

    virtual bool Add(const Cuma::Address::AddressBlock& AddressBlock, const QString& FileName, const QString Pid);

    virtual bool RemoveFrom(const QString ip);

    virtual bool RemoveTo (const QString ip);

    virtual bool RemoveFromFileAddress (const QString FileName, const QString Pid);

    virtual Cuma::Address::AddressBlock GetAddress(const QString FileName, const QString Pid);

    virtual QVector <Cuma::Address::AddressBlock> GetAddress(const QString FileName);

    virtual QString GetErrorString();

    virtual QSqlError GetError();

protected:
    QSqlDatabase AddressDb;
    QString ErrorString;
    QSqlError Error;

};

}

}

#endif // DBADDRESSPATHBYFILE_H
