#ifndef SPREADHANDLER_H
#define SPREADHANDLER_H

#include <QObject>

#include <QCryptographicHash>

#include "Log/debuglog.h"
#include "Log/ErrorLog.h"
#include "Protocol/protocol.h"
#include "FireWall/accessclientfirewall.h"
#include "Block/AddressBlock/addressblock.h"
#include "Block/FileBlock/blockstruct.h"
#include "DbAddressPath/dbaddresspathbyfile.h"
#include "DbFileFragInfo/dbcache.h"
#include "FileBlockStorage/filefrag.h"

class SpreadHandler : public QObject
{
    Q_OBJECT
public:
    explicit SpreadHandler(QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                           QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbAddressPath,
                           QCryptographicHash::Algorithm& Algorithm,
                           QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage,
                           Cuma::Protocol::CumaProtocolBlock& Block);

    ~SpreadHandler();

    bool IsSuccess();

    Cuma::FileBlock::FileBlock GetFileBlockToSpread();

    static Cuma::Protocol::CumaProtocolBlock MakeReplyFail(const Cuma::Protocol::CumaProtocolBlock& block, const QString ErrorMessage);

    static Cuma::Protocol::CumaProtocolBlock MakeReplySuccess(const Cuma::Protocol::CumaProtocolBlock& block);

protected:
    Cuma::FileBlock::FileFragInfo MakeFileFragInfo(Cuma::FileBlock::FileBlock RecvFileBlock, QByteArray FileBlockHash);

protected:
    bool IsSaved;

    Cuma::FileBlock::FileBlock ReadySpreadFileBlock;

};

#endif // SPREADHANDLER_H
