#include "dbcache.h"

Cuma::DbFileFrag::DbFileFragInfo::DbFileFragInfo(QObject* parent)
{
    if (QSqlDatabase::contains("FileFragDBCache") )
    {
        db = QSqlDatabase::database("FileFragDBCache");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", "FileFragDBCache");
    }

    db.setDatabaseName("FileFragDBCache.db");
    db.open();

    QSqlQuery CheckTableQuery = db.exec("SELECT tbl_name AS temp FROM sqlite_master");

    CheckTableQuery.next();

    if (CheckTableQuery.value(0).toString() == "FileFragCache")
    {
        return;
    }

    QSqlQuery query = db.exec("CREATE TABLE `FileFragCache` ("
                              "`FileName`	TEXT,"
                              "`FileBlockPid`	TEXT,"
                              "`FileIndex`	INTEGER,"
                              "`FilePath`	TEXT,"
                              "PRIMARY KEY(`FileBlockPid`));");
    if (query.isActive() == false)
    {
        this->Error = db.lastError();
        this->ErrorStr = db.lastError().text();
    }
}

Cuma::DbFileFrag::DbFileFragInfo::DbFileFragInfo(QSqlDatabase Database)
{
    db = Database;
}

Cuma::DbFileFrag::DbFileFragInfo::~DbFileFragInfo()
{
    db.close();
}

QVector<QString> Cuma::DbFileFrag::DbFileFragInfo::GetAllFileList()
{
    QVector<QString> RetValue;

    if (db.isValid() == false || db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return RetValue;
    }

    //FileBlock의 위치를 검색
    QSqlQuery query( db);
    if (query.exec("SELECT * FROM `FileFragCache`") == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        return RetValue;
    }

    QSqlRecord ResultQuery = query.record();
    int FileNameIndex = ResultQuery.indexOf("FileName");
    int FileIndexIndex = ResultQuery.indexOf("FileIndex");

    QString FileName;
    unsigned int FileIndex;
    while (query.next())
    {
        FileName = query.value(FileNameIndex).toString();
        FileIndex = query.value(FileIndexIndex).toInt();

        RetValue.append(FileName + QString::number(FileIndex));
    }

    return RetValue;
}

bool Cuma::DbFileFrag::DbFileFragInfo::AddFileFrag(Cuma::FileBlock::FileFragInfo FileBlock, QUrl Path)
{
    if (db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return false;
    }

    if (FileBlock.FileName.isEmpty() ||
            FileBlock.FilePid.isEmpty() ||
            FileBlock.Index == -1 ||
            Path.isEmpty())

    {
        qDebug() << "[Error] : FileBlock is empty";
        return false;
    }

    //FileBlock의 위치를 검색
    QSqlQuery query(db);
    query.prepare("SELECT * FROM `FileFragCache` WHERE `FileBlockPid` = (:FBlockPid)");

    query.bindValue(":FBlockPid", FileBlock.FilePid);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Search " + ErrorStr + " QSqlDB : " + db.lastError().text();
        return false;
    }

    if (query.last() != QSql::AfterLastRow && query.at() + 1 > 0)
    {
        qDebug() << "[Debug] : File is Exsist";
        return true;
    }

    if (query.prepare("INSERT INTO `FileFragCache` (`FileName`,`FileBlockPid`,`FileIndex`,`FilePath`) VALUES ( :FName , :FBlockPid  , :FIndex , :FPath); ") == false)
    {
        Error = query.lastError();
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Prepare is fail"  + ErrorStr + " QSqlDB : " + db.lastError().text();
        return false;
    }

    query.bindValue(":FName", FileBlock.FileName);
    query.bindValue(":FBlockPid", FileBlock.FilePid);
    query.bindValue(":FIndex", FileBlock.Index);
    query.bindValue(":FPath", Path.toString());

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : " + ErrorStr + " QSqlDB : " + db.lastError().text();
        return false;
    }

    return true;
}

QVector<QUrl> Cuma::DbFileFrag::DbFileFragInfo::SearchFile(QString FileName)
{
    QVector<QUrl> RetUrl;

    if (db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return RetUrl;
    }

    //FileBlock의 위치를 검색
    QSqlQuery query(db);
    if (query.prepare("SELECT `FilePath` From `FileFragCache` WHERE `FileName` = ( :FName) ") == false)
    {
        Error = query.lastError();
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Prepare is fail"  + ErrorStr + " QSqlDB : " + db.lastError().text();
        return RetUrl;
    }

    query.bindValue(":FName", FileName);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Prepare is fail"  + ErrorStr + " QSqlDB : " + db.lastError().text();
        return RetUrl;
    }

    while (query.next())
    {
        QString FilePath = query.value(0).toString();
        RetUrl.append(FilePath);
    }

    return RetUrl;
}

QUrl Cuma::DbFileFrag::DbFileFragInfo::SearchFile(Cuma::FileBlock::FileFragInfo block)
{
    QUrl RetUrl;

    if (db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return RetUrl;
    }


    //FileBlock의 위치를 검색
    QSqlQuery query(db);
    if (query.prepare("SELECT `FilePath` From `FileFragCache`WHERE (`FileName` = :FileName) AND (`FileIndex` = :FileIndex)") == false)
    {
        Error = query.lastError();
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Prepare is fail"  + ErrorStr + " QSqlDB : " + db.lastError().text();
        return RetUrl;
    }

    query.bindValue(":FileName", block.FileName);
    query.bindValue(":FileIndex", block.Index);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        return RetUrl;
    }

    int FilePathIndex = query.record().indexOf("FilePath");
    query.next();

    RetUrl = QUrl( query.value(FilePathIndex).toString());

    return RetUrl;
}

Cuma::FileBlock::FileFragInfo Cuma::DbFileFrag::DbFileFragInfo::SearchFileInfo(QString Pid)
{
    Cuma::FileBlock::FileFragInfo FileUnitInfo;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM `FileFragCache` WHERE `FileBlockPid` = (:FileBlockPid)");

    query.bindValue(":FileBlockPid", Pid);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Search " + ErrorStr + " QSqlDB : " + db.lastError().text();
        return FileUnitInfo;
    }

    query.next();

    FileUnitInfo.FileName = query.value(0).toString();
    FileUnitInfo.FilePid = query.value(1).toString();
    FileUnitInfo.Index = static_cast<unsigned int>(query.value(2).toInt());

    return FileUnitInfo;
}

QVector<Cuma::FileBlock::FileFragInfo> Cuma::DbFileFrag::DbFileFragInfo::SearchFileInfo(QString FileName, unsigned int FileIndex)
{
    QVector<Cuma::FileBlock::FileFragInfo> ReturnFileFragInfoList;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM `FileFragCache` WHERE `FileName` = (:FileName) AND `FileIndex` = (:FBlockIndex) ");

    query.bindValue(":FBlockIndex", FileIndex);
    query.bindValue(":FileName", FileName);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        qDebug() << "[Error] : Search " + ErrorStr + " QSqlDB : " + db.lastError().text();
        return ReturnFileFragInfoList;
    }

    while (query.next())
    {
        Cuma::FileBlock::FileFragInfo FileUnitInfo;

        FileUnitInfo.FileName = query.value(0).toString();
        FileUnitInfo.FilePid = query.value(1).toString();
        FileUnitInfo.Index = static_cast<unsigned int>(query.value(2).toInt());

        ReturnFileFragInfoList.append(FileUnitInfo);
    }

    return ReturnFileFragInfoList;
}

bool Cuma::DbFileFrag::DbFileFragInfo::RemoveFile(QString FileName)
{
    if (db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return false;
    }

    //FileBlock의 위치를 검색
    QSqlQuery query( db);
    query.prepare("DELETE FROM `FileFragCache`"
                  "WHERE `FileName` IN ( :FName ); ");
    query.bindValue(":FName", FileName);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        return false;
    }

    return true;
}

bool Cuma::DbFileFrag::DbFileFragInfo::RemoveFile(Cuma::FileBlock::FileFragInfo block)
{
    if (db.isOpen() == false)
    {
        qDebug() << "[Error] Can't find QSqlDatabase instance";
        ErrorStr = "Can't find QSqlDatabase instance";

        return false;
    }

    //FileBlock의 위치를 검색
    QSqlQuery query( db);
    query.prepare("DELETE FROM `FileFragCache` WHERE `FileIndex` IN ( :FIndex) AND `FileName` IN ( :FName)");

    query.bindValue(":FName", block.FileName);
    query.bindValue(":FIndex", block.Index);

    if (query.exec() == false)
    {
        ErrorStr = "QSqlQuery exec is fail : " + query.lastError().text();
        return false;
    }

    return true;
}

QSqlError Cuma::DbFileFrag::DbFileFragInfo::GetError()
{
    return Error;
}

QString Cuma::DbFileFrag::DbFileFragInfo::GetErrorString()
{
    return ErrorStr;
}
