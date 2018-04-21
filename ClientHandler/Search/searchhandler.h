#ifndef SEARCHHANDLER_H
#define SEARCHHANDLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "DbFileFragInfo/dbcache.h"
#include "Protocol/protocol.h"
#include "Block/FileBlock/blockstruct.h"
#include "Block/AddressBlock/addressblock.h"

class SearchHandler : public QObject
{
    Q_OBJECT
public:
    explicit SearchHandler(QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                           const Cuma::Protocol::CumaProtocolBlock& RequestProtocol);

    ~SearchHandler();

    bool isFind();

    static Cuma::Protocol::CumaProtocolBlock MakeReplySuccess(Cuma::Protocol::CumaProtocolBlock block);

    static Cuma::Protocol::CumaProtocolBlock MakeReplyFail(Cuma::Protocol::CumaProtocolBlock block);

    QVector<Cuma::FileBlock::FileFragInfo> FileFragInfoList;

    Cuma::FileBlock::FileFragInfo FileFragInfo;

private:
    bool isFindCache;

};

#endif // SEARCHHANDLER_H
