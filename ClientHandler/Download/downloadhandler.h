#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QUrl>
#include <QSharedPointer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include "DbFileFragInfo/dbcache.h"
#include "FileBlockStorage/filefrag.h"
#include "Protocol/protocol.h"
#include "Block/FileBlock/blockstruct.h"
#include "Block/AddressBlock/addressblock.h"
#include "Log/debuglog.h"
#include "Log/errorlog.h"

class DownloadHandler : public QObject
{
    Q_OBJECT
public:
    explicit DownloadHandler(QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                             QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage,
                             Cuma::Protocol::CumaProtocolBlock Block);
    ~DownloadHandler();

    QByteArray FileResource;

    bool isSuccess;

    bool isFileNotExsist;

    bool isFileBlockParseFail;

    bool isFileParsingError;

    bool isReadFileError;

    static Cuma::Protocol::CumaProtocolBlock MakeReplySuccess(Cuma::Protocol::CumaProtocolBlock protocol, QByteArray FileSource, Cuma::FileBlock::FileFragInfo info);

    static Cuma::Protocol::CumaProtocolBlock MakeReplyFail(Cuma::Protocol::CumaProtocolBlock protocol);
protected:
    QByteArray ReadFileFragDir(QUrl FileSaveLocation);

};

#endif // DOWNLOAD_H
