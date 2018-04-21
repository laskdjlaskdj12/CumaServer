//#ifndef SPREADHANDLETEST_H
//#define SPREADHANDLETEST_H

//#include <QObject>
//#include <QJsonDocument>
//#include <QTest>
//#include <QDir>
//#include <QCryptographicHash>
//#include <QSharedPointer>
//#include <QUrl>

//#include "ClientHandler/Spread/spreadhandler.h"
//#include "Log/debuglog.h"

////Dependency
//#include "Protocol/protocol.h"
//#include "FireWall/accessclientfirewall.h"
//#include "DbAddressPath/dbaddresspathbyfile.h"
//#include "FileBlockStorage/filefrag.h"
//#include "Block/FileBlock/blockstruct.h"

//class SpreadHandleTest: public QObject
//{
//    Q_OBJECT
//public:
//    SpreadHandleTest()
//    {

//    }

//    ~SpreadHandleTest()
//    {

//    }

//    void InitTestDirectory(QDir dir)
//    {
//        dir.mkdir("SpreadHandlerTestDir");
//        dir.cd("SpreadHandlerTestDir");

//        QDir::setCurrent(dir.absolutePath());
//    }

//    void DeInitTestDirectory(QDir dir)
//    {
//        dir.removeRecursively();
//        dir.cdUp();

//        QDir::setCurrent(dir.absolutePath());
//    }

//    bool isFileBlocksSame(Cuma::FileBlock::FileBlock FileBlock_1, Cuma::FileBlock::FileBlock FileBlock_2)
//    {
//        return (FileBlock_1.FileFragCount == FileBlock_2.FileFragCount) &&
//               (FileBlock_1.FileName == FileBlock_2.FileName) &&
//               (FileBlock_1.FileSize == FileBlock_2.FileSize) &&
//               (FileBlock_1.FileSource == FileBlock_2.FileSource) &&
//               (FileBlock_1.Index == FileBlock_2.Index) &&
//               (FileBlock_1.Version == FileBlock_2.Version);
//    }

//    Cuma::Protocol::CumaProtocolBlock makeTestFileBlock()
//    {
//        Cuma::FileBlock::FileBlock FileBlock;
//        FileBlock.FileFragCount = 1;
//        FileBlock.FileName = "Test.txt";
//        FileBlock.FileSource = "This is FileSource";
//        FileBlock.FileSize = QString("This is FileSource").size();
//        FileBlock.Index = 0;
//        FileBlock.Version = 1.0;

//        Cuma::Protocol::CumaProtocolBlock ReturnBlock;
//        ReturnBlock.Address.From.IP = "127.0.0.1";
//        ReturnBlock.Address.From.Port = 7070;

//        ReturnBlock.Address.To.IP = "127.0.0.1";
//        ReturnBlock.Address.To.Port = 7071;

//        ReturnBlock.Address.Direction = Cuma::Address::Req;
//        ReturnBlock.Data = QJsonDocument(Cuma::FileBlock::Serlize::FileBlockToJson(FileBlock)).toJson();

//        return ReturnBlock;
//    }

//private slots:

//    void AddSpreadHandler()
//    {
//        try
//        {
//            QDir dir;

//            InitTestDirectory(dir);

//            Cuma::Protocol::CumaProtocolBlock Block = makeTestFileBlock();
//            Block.Address.From.IP = "127.0.0.1";

//            QSharedPointer<Cuma::DbAddress::DbAddressPathByFile> DbAddressPath = QSharedPointer<Cuma::DbAddress::DbAddressPathByFile>::create();
//            QSharedPointer<Cuma::FileBlockStorage::FileFragDir> FileBlockStorage = QSharedPointer<Cuma::FileBlockStorage::FileFragDir>::create();
//            QString SoragePath = QDir::currentPath();
//            FileBlockStorage->SetFileSaveLocation(QUrl(SoragePath));

//            QCryptographicHash::Algorithm RequestAlgorithum = QCryptographicHash::Sha3_512;

//            SpreadHandler handler(DbAddressPath, RequestAlgorithum, FileBlockStorage, Block);

//            QVERIFY (handler.IsSuccess());

//            Cuma::FileBlock::FileBlock FileBlock = handler.GetFileBlockToSpread();

//            QVERIFY (FileBlock.FileName != "Exsist");
//            QVERIFY (FileBlock.FileName.isEmpty() == false);
//            QVERIFY (FileBlock.FileSource.isEmpty() == false);
//            QVERIFY (FileBlock.FileSize > 0);
//            QVERIFY (FileBlock.FileFragCount == 1);
//            QVERIFY (FileBlock.Index == 0);

//            Cuma::FileBlock::FileBlock RecvBlock = Cuma::FileBlock::Serlize::JsonToFileBlock(QJsonDocument::fromJson(Block.Data).object());
//            QByteArray FilePid = QCryptographicHash::hash(RecvBlock.FileSource, QCryptographicHash::Sha3_512);

//            Cuma::Address::AddressBlock SavedAddressBlock = DbAddressPath->GetAddress("Test.txt", FilePid);

//            if (Block.Address.From.IP.isEmpty())
//            {
//                DEBUGLOG("SavedAddressBlock이 Empty입니다.");
//                QVERIFY2(Block.Address.From.IP.isEmpty() == false, DbAddressPath->GetErrorString().toStdString().c_str());
//            }

//            QVERIFY(Block.Address.From.IP == SavedAddressBlock.From.IP);
//            QVERIFY(Block.Address.To.IP == SavedAddressBlock.To.IP);
//            QVERIFY(Block.Address.PathCount == SavedAddressBlock.PathCount);

//            Cuma::FileBlock::FileBlock FromFileStorageBlock = FileBlockStorage->GetFileBlock("Test.txt", 0);

//            QVERIFY (isFileBlocksSame(RecvBlock, FromFileStorageBlock));

//            DeInitTestDirectory(dir);
//        }
//        catch (QSqlError& e)
//        {
//            DEBUGLOG(e.text());
//        }
//    }
//};
//#endif // SPREADHANDLETEST_H
