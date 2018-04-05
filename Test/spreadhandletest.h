#ifndef SPREADHANDLETEST_H
#define SPREADHANDLETEST_H

#include <QObject>
#include <QJsonDocument>
#include <QTest>
#include <QDir>
#include <QCryptographicHash>
#include <QSharedPointer>

#include "ClientHandler/Spread/spreadhandler.h"

//Dependency
#include "Protocol/protocol.h"
#include "FireWall/accessclientfirewall.h"
#include "DbAddressPath/dbaddresspathbyfile.h"
#include "FileBlockStorage/filefrag.h"
#include "Block/FileBlock/blockstruct.h"

class SpreadHandleTest: public QObject
{
public:
    SpreadHandleTest()
    {

    }

    ~SpreadHandleTest()
    {

    }


private slots:
    void InitTestDirectory(QDir dir)
    {
        dir.mkdir("SpreadHandlerTestDir");
        dir.cd("SpreadHandlerTestDir");
        QDir::setCurrent(dir.absolutePath());
    }

    void DeInitTestDirectory(QDir dir)
    {
        dir.cdUp();

        QDir::setCurrent(dir.absolutePath());
    }

    bool isFileBlocksSame(Cuma::FileBlock::FileBlock FileBlock_1, Cuma::FileBlock::FileBlock FileBlock_2)
    {
        return false;
    }

    void AddSpreadHandler()
    {
        QDir dir;

        InitTestDirectory(dir);

        Cuma::Protocol::CumaProtocolBlock Block;
        Block.Address.From.IP = "127.0.0.1";

        QSharedPointer<Cuma::DbAddress::DbAddressPathByFile> DbAddressPath = QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>::create();
        QSharedPointer<Cuma::FileBlockStorage::FileFragDir> FileBlockStorage = QSharedPointer<Cuma::FileBlockStorage::FileFragDir>::create();
        QCryptographicHash::Algorithm RequestAlgorithum = QCryptographicHash::Sha3_512;

        SpreadHandler handler(DbAddressPath, RequestAlgorithum, FileBlockStorage, Block);

        QVERIFY (handler.IsSuccess());

        Cuma::FileBlock::FileBlock FileBlock = handler.GetFileBlockToSpread();

        QVERIFY (FileBlock.FileName.isEmpty() == false);
        QVERIFY (FileBlock.FileSource.isEmpty() == false);
        QVERIFY (FileBlock.FileSize > 0);
        QVERIFY (FileBlock.FileFragCount == 1);
        QVERIFY (FileBlock.Index == 0);

        Cuma::FileBlock::FileBlock RecvBlock = Cuma::FileBlock::Serlize::JsonToFileBlock(QJsonDocument::fromJson(Block.Data).object());
        QByteArray FilePid = QCryptographicHash::hash(RecvBlock.FileSource, QCryptographicHash::Sha3_512);

        Cuma::Address::AddressBlock SavedAddressBlock = DbAddressPath->GetAddress("test.txt", FilePid);
        QVERIFY(Block.Address == SavedAddressBlock);

        Cuma::FileBlock::FileBlock FromFileStorageBlock = FileBlockStorage->GetFileBlock("test.txt", 0);

        QVERIFY (isFileBlocksSame(RecvBlock, FromFileStorageBlock));

        DeInitTestDirectory(dir);
    }
};
#endif // SPREADHANDLETEST_H
