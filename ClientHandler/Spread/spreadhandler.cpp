#include "spreadhandler.h"

SpreadHandler::SpreadHandler(QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbPath,
                             QCryptographicHash::Algorithm& hashType,
                             QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage,
                             Cuma::Protocol::CumaProtocolBlock& FileBlockProtocol):
    IsSaved(false)
{
    Cuma::FileBlock::FileBlock RecvFileBlock;

    RecvFileBlock = Cuma::FileBlock::Serlize::JsonToFileBlock(QJsonDocument::fromJson(FileBlockProtocol.Data).object());

    QByteArray FileBlockHash = QCryptographicHash::hash(RecvFileBlock.FileSource, hashType);

    Cuma::Address::AddressBlock FileBlockSenderAddress = DbPath->GetAddress(RecvFileBlock.FileName, QCryptographicHash::hash(RecvFileBlock.FileSource, hashType));

    if (FileBlockSenderAddress.From.IP.isEmpty() == false)
    {
        IsSaved = true;
        ReadySpreadFileBlock.FileName = "Exsist";
        return;
    }

    if (DbPath->Add(FileBlockProtocol.Address, RecvFileBlock.FileName, FileBlockHash) == false)
    {
        ERRLOG("파일의 정보를 저장할수 없습니다.");
        IsSaved = false;
        return;
    }

    //파일을 블록에 저장함
    if (FileBlockStorage->SaveFileBlock(RecvFileBlock) == false)
    {
        ERRLOG("파일 소스를 저장할수없습니다.");
        IsSaved = false;
        return;
    }

    //파일블록이 저장이 되어있는지 하는 플래그를 true함
    IsSaved = true;

    ReadySpreadFileBlock = RecvFileBlock;
}

SpreadHandler::~SpreadHandler()
{

}

bool SpreadHandler::IsSuccess()
{
    return IsSaved;
}

Cuma::FileBlock::FileBlock SpreadHandler::GetFileBlockToSpread()
{
    return ReadySpreadFileBlock;
}

Cuma::Protocol::CumaProtocolBlock SpreadHandler::MakeReplyFail(const Cuma::Protocol::CumaProtocolBlock& block, const QString ErrorMessage)
{
    Cuma::Protocol::CumaProtocolBlock ReturnBlock;
    ReturnBlock.Address = Cuma::Address::ChangeAddressDirection(block.Address);
    QJsonObject ErrorObj;
    ErrorObj["Error"] = ErrorMessage;

    ReturnBlock.Data = QJsonDocument(ErrorObj).toJson();

    return ReturnBlock;
}

Cuma::Protocol::CumaProtocolBlock SpreadHandler::MakeReplySuccess(const Cuma::Protocol::CumaProtocolBlock& block)
{
    Cuma::Protocol::CumaProtocolBlock ReturnBlock;
    ReturnBlock.Address = Cuma::Address::ChangeAddressDirection(block.Address);
    QJsonObject SuccessObj;
    SuccessObj["Success"] = "SuccessSave";

    ReturnBlock.Data = QJsonDocument(SuccessObj).toJson();

    return ReturnBlock;
}
