#include "downloadhandler.h"

DownloadHandler::DownloadHandler(QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                                 QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage,
                                 Cuma::Protocol::CumaProtocolBlock RequestProtocol):
    isSuccess(false),
    isFileNotExsist(false),
    isFileParsingError(false),
    isReadFileError(false)
{

    QJsonObject RequestFileFragInfoJson = QJsonDocument::fromJson(RequestProtocol.Data).object();

    //파일에서 Block이 있는지 확인
    Cuma::FileBlock::FileFragInfo ReqFileBlockInfo = Cuma::FileBlock::Serlize::JsonToFileInfo(RequestFileFragInfoJson);

    //만약 Block가 없을경우
    if (ReqFileBlockInfo.FileName.isEmpty() ||
            ReqFileBlockInfo.FilePid.isEmpty() ||
            ReqFileBlockInfo.Index < 0)
    {
        DEBUGLOG("파일을 파싱할수 없습니다.");
        isFileBlockParseFail = true;
        return;
    }

    //Block을 검색해서 로드
    QUrl FileSaveLocation = DbFileFragInfo->SearchFile(ReqFileBlockInfo);

    FileResource = ReadFileFragDir(ReqFileBlockInfo.FileName, ReqFileBlockInfo.Index, FileSaveLocation);

    if (FileResource.isEmpty())
    {
        isSuccess = false;
        return;
    }

    isSuccess = true;
}

DownloadHandler::~DownloadHandler()
{

}

Cuma::Protocol::CumaProtocolBlock DownloadHandler::MakeReplySuccess(Cuma::Protocol::CumaProtocolBlock protocol,
        QByteArray FileSource,
        Cuma::FileBlock::FileFragInfo info)
{
    Cuma::FileBlock::FileBlock Block;

    Block.Version = 1.0;
    Block.FileName = info.FileName;
    Block.FileSize = FileSource.size();
    Block.Index = info.Index;
    Block.FileSource = FileSource;

    QJsonObject DataObj;
    DataObj = Cuma::FileBlock::Serlize::FileBlockToJson(Block);
    DataObj["Success"] = true;

    Cuma::Protocol::CumaProtocolBlock returnProtocol;
    returnProtocol.Address = Cuma::Address::ChangeAddressDirection(protocol.Address);
    returnProtocol.Data = QJsonDocument(DataObj).toJson();

    return returnProtocol;
}

Cuma::Protocol::CumaProtocolBlock DownloadHandler::MakeReplyFail(Cuma::Protocol::CumaProtocolBlock protocol)
{
    QJsonObject DataObj;
    DataObj["Success"] = false;

    Cuma::Protocol::CumaProtocolBlock returnProtocol;
    returnProtocol.Address = Cuma::Address::ChangeAddressDirection(protocol.Address);
    returnProtocol.Data = QJsonDocument(DataObj).toJson();

    return returnProtocol;
}

QByteArray DownloadHandler::ReadFileFragDir(QString FileName, unsigned int Index, QUrl FileSaveLocation)
{
    //해당 파일을 open함
    QString DownloadFileName = FileName + QString::number(Index) + ".Cuma";

    QFile CumaFile(FileSaveLocation.toString() + "/" + DownloadFileName);

    QByteArray FileSource;

    //만약 해당 파일이 오픈되지 않을경우
    if (CumaFile.open(QFile::ReadOnly) == false)
    {
        DEBUGLOG("파일을 열수 없습니다. : " + CumaFile.errorString());
        CumaFile.close();
        isFileNotExsist = true;
        return FileSource;
    }

    //해당 파일의 소스들을 읽어서 리턴함
    FileSource = CumaFile.readAll();

    //CumaFile을 Close함
    CumaFile.close();

    return FileSource;
}
