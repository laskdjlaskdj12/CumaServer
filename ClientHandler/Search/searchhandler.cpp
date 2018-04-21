#include "searchhandler.h"

SearchHandler::SearchHandler(QSharedPointer<Cuma::DbFileFrag::DbFileFragInfo>& DbFileFragInfo,
                             const Cuma::Protocol::CumaProtocolBlock& RequestProtocol):
    isFindCache(false)
{

    QJsonObject JsonFileFragInfo = QJsonDocument::fromJson(RequestProtocol.Data).object();

    Cuma::FileBlock::FileFragInfo ReqSearchFileFragInfo = Cuma::FileBlock::Serlize::JsonToFileInfo(JsonFileFragInfo);

    if (DbFileFragInfo->SearchFile(ReqSearchFileFragInfo).isEmpty())
    {
        isFindCache = false;
    }
    else
    {
        isFindCache = true;
    }
}

SearchHandler::~SearchHandler()
{

}

bool SearchHandler::isFind()
{
    return isFindCache;
}

Cuma::Protocol::CumaProtocolBlock SearchHandler::MakeReplySuccess(Cuma::Protocol::CumaProtocolBlock block)
{
    QJsonObject ProtocolDataObj;
    ProtocolDataObj["Success"] = true;

    Cuma::Protocol::CumaProtocolBlock returnBlock;

    returnBlock.Address = Cuma::Address::ChangeAddressDirection(block.Address);
    returnBlock.Data = QJsonDocument (ProtocolDataObj).toJson();
    returnBlock.ProtocolType = Cuma::Protocol::Search;

    return returnBlock;
}

Cuma::Protocol::CumaProtocolBlock SearchHandler::MakeReplyFail(Cuma::Protocol::CumaProtocolBlock block)
{
    QJsonObject ProtocolDataObj;
    ProtocolDataObj["Success"] = false;

    Cuma::Protocol::CumaProtocolBlock returnBlock;

    returnBlock.Address = Cuma::Address::ChangeAddressDirection(block.Address);
    returnBlock.Data = QJsonDocument (ProtocolDataObj).toJson();
    returnBlock.ProtocolType = Cuma::Protocol::Search;

    return returnBlock;
}
