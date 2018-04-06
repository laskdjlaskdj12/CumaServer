#include "spreadhandler.h"

SpreadHandler::SpreadHandler(QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>& DbPath,
                             QCryptographicHash::Algorithm& hashType,
                             QSharedPointer<Cuma::FileBlockStorage::FileFragDir>& FileBlockStorage,
                             Cuma::Protocol::CumaProtocolBlock& FileBlockProtocol):
    IsSaved(false)
{
    Cuma::FileBlock::FileBlock RecvFileBlock;

    try
    {
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
            throw ("파일의 정보를 저장할수 없습니다.");
        }

        //파일을 블록에 저장함
        if (FileBlockStorage->SaveFileBlock(RecvFileBlock) == false)
        {
            ERRLOG("파일 소스를 저장할수없습니다.");
            throw ("파일소스를 저장할수없습니다.");
        }

        //파일블록을 다른 서버에 퍼트리는 역활을 함
        IsSaved =  true;

        ReadySpreadFileBlock = RecvFileBlock;
    }
    catch (const QString String)
    {
        ERRLOG(String);
        IsSaved = false;
    }
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
