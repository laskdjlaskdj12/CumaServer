#include "dbaddresspathbyfile.h"

Cuma::DbAddress::DbAddressPathByFile::DbAddressPathByFile (QSqlDatabase DB)
{
    AddressDb = DB;
}

Cuma::DbAddress::DbAddressPathByFile::DbAddressPathByFile()
{
    AddressDb = QSqlDatabase::addDatabase("QSQLITE", "DBAddressPath");
    AddressDb.setDatabaseName("DBAddressPath.db");

    if (AddressDb.open() == false)
    {
        this->Error = AddressDb.lastError();

        return;
    }

    //테이블이 있는지 체크

    QSqlQuery CheckTableQuery = AddressDb.exec("SELECT tbl_name AS temp FROM sqlite_master");

    CheckTableQuery.next();

    if (CheckTableQuery.value(0).toString() == "FileFragAddress")
    {
        return;
    }

    QSqlQuery query = AddressDb.exec("CREATE TABLE `FileFragAddress` ("
                                     "`FromIP`	TEXT,"
                                     "`FromPort`	INTEGER,"
                                     "`ToIp`	TEXT,"
                                     "`ToPort`	INTEGER,"
                                     "`Direction`	NUMERIC,"
                                     "`BypassArray`	TEXT,"
                                     "`PathCount`	INTEGER,"
                                     "`FileName`	TEXT,"
                                     "`Pid`	TEXT NOT NULL,"
                                     "PRIMARY KEY(`Pid`));");

    if (query.isActive() == false)
    {
        this->Error = AddressDb.lastError();
    }
}

Cuma::DbAddress::DbAddressPathByFile::~DbAddressPathByFile()
{
    if (AddressDb.isOpen() )
    {
        AddressDb.close();
    }
}

bool Cuma::DbAddress::DbAddressPathByFile::Add(const Cuma::Address::AddressBlock& AddressBlock, const QString& FileName, const QString Pid)
{
    if (AddressDb.isOpen() == false)
    {
        qDebug() << "[Error] : Init QSqlDatabase is not set";
        Error = AddressDb.lastError();
        ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
        return false;
    }

    QSqlQuery query(AddressDb);

    query.prepare("INSERT INTO `FileFragAddress`(`FromIP`, `FromPort`, `ToIp`, `ToPort`, `Direction`, `BypassArray`, `PathCount`, `FileName`, `Pid`)"
                  " VALUES (:FromIP , :FromPort , :ToIp , :ToPort , :Direction , :BypassArray , :PathCount , :FileName , :Pid);");

    query.bindValue(":FromIP", AddressBlock.From.IP );
    query.bindValue(":FromPort", AddressBlock.From.Port );
    query.bindValue(":ToIp", AddressBlock.To.IP );
    query.bindValue(":ToPort", AddressBlock.To.Port);
    query.bindValue(":Direction", AddressBlock.Direction );
    query.bindValue(":BypassArray", QJsonDocument(AddressBlock.BypassArray).toJson() );
    query.bindValue(":PathCount", AddressBlock.PathCount);
    query.bindValue(":FileName", FileName);
    query.bindValue(":Pid", Pid);

    if (query.exec() == false)
    {
        qDebug() << "[Error] : Add Query is fail : " + query.lastError().text();
        Error = AddressDb.lastError();
        ErrorString = "Add Query is fail: " + query.lastError().text();
        return false;
    }

    return true;
}

bool Cuma::DbAddress::DbAddressPathByFile::RemoveFrom(const QString ip)
{
    if (AddressDb.isOpen() == false)
    {
        qDebug() << "[Error] : Init QSqlDatabase is not set";
        Error = AddressDb.lastError();
        ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
        return false;
    }

    QSqlQuery query(AddressDb);

    query.prepare("DELETE FROM `FileFragAddress` WHERE `FromIP` = (:ip) ");

    query.bindValue(":ip", ip);

    if (query.exec() == false)
    {
        qDebug() << "[Error] : Add Query is fail";
        Error = AddressDb.lastError();
        ErrorString = "Add Query is fail: " + query.lastError().text();
        return false;
    }

    return true;
}

bool Cuma::DbAddress::DbAddressPathByFile::RemoveTo(const QString ip)
{
    if (AddressDb.isOpen() == false)
    {
        qDebug() << "[Error] : Init QSqlDatabase is not set";
        Error = AddressDb.lastError();
        ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
        return false;
    }

    QSqlQuery query(AddressDb);

    query.prepare("DELETE FROM `FileFragAddress` WHERE `ToIP` = (:ip) ");

    query.bindValue(":ip", ip);

    if (query.exec() == false)
    {
        qDebug() << "[Error] : Add Query is fail";
        Error = AddressDb.lastError();
        ErrorString = "Add Query is fail: " + query.lastError().text();
        return false;
    }

    return true;
}

bool Cuma::DbAddress::DbAddressPathByFile::RemoveFromFileAddress(const QString FileName, const QString Pid)
{
    if (AddressDb.isOpen() == false)
    {
        qDebug() << "[Error] : Init QSqlDatabase is not set";
        Error = AddressDb.lastError();
        ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
        return false;
    }

    QSqlQuery query(AddressDb);

    query.prepare("DELETE FROM `FileFragAddress` WHERE `FileName` = (:FileName) AND `Pid` = (:FileBlockPid) ");

    query.bindValue(":FileName", FileName);
    query.bindValue(":FileBlockPid", Pid);

    if (query.exec() == false)
    {
        qDebug() << "[Error] : Add Query is fail";
        Error = AddressDb.lastError();
        ErrorString = "Add Query is fail: " + query.lastError().text();
        return false;
    }

    return true;
}

Cuma::Address::AddressBlock Cuma::DbAddress::DbAddressPathByFile::GetAddress(const QString FileName, const QString Pid)
{

    try
    {
        if (AddressDb.isOpen() == false)
        {
            qDebug() << "[Error] : Init QSqlDatabase is not set";
            throw AddressDb.lastError();
        }

        QSqlQuery query(AddressDb);

        query.prepare("SELECT * FROM `FileFragAddress` WHERE `FileName` = (:FileName) AND `Pid` = (:FileBlockPid) ");

        query.bindValue(":FileName", FileName);
        query.bindValue(":FileBlockPid", Pid);

        if (query.exec() == false)
        {
            qDebug() << "[Error] : Add Query is fail";
            Error = AddressDb.lastError();
            ErrorString = "Add Query is fail: " + query.lastError().text();
            throw AddressDb.lastError();
        }

        query.next();

        Cuma::Address::AddressBlock RetBlock;

        RetBlock.From.IP = query.value(0).toString();
        RetBlock.From.Port = query.value(1).toInt();
        RetBlock.To.IP = query.value(2).toString();
        RetBlock.To.Port = query.value(3).toInt();
        RetBlock.Direction = Cuma::Address::BypassDirection(query.value(4).toInt());
        RetBlock.BypassArray = QJsonDocument::fromJson(query.value(5).toString().toUtf8()).array();
        RetBlock.PathCount = query.value(6).toInt();

        return RetBlock;
    }

    catch (const QSqlError& e)
    {
        qDebug() << "Error of Sql : " + e.text();

        Cuma::Address::AddressBlock EmptyBlock;
        return EmptyBlock;
    }
}

QVector<Cuma::Address::AddressBlock> Cuma::DbAddress::DbAddressPathByFile::GetAddress(QString FileName)
{
    QVector<Cuma::Address::AddressBlock> RetBlockList;

    try
    {
        if (AddressDb.isOpen() == false)
        {
            qDebug() << "[Error] : Init QSqlDatabase is not set";
            Error = AddressDb.lastError();
            ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
            throw false;
        }

        QSqlQuery query(AddressDb);

        query.prepare("SELECT * FROM `FileFragAddress` WHERE `FileName` = (:FileName) ");

        query.bindValue(":FileName", FileName);

        if (query.exec() == false)
        {
            qDebug() << "[Error] : Add Query is fail";
            Error = AddressDb.lastError();
            ErrorString = "Add Query is fail: " + query.lastError().text();
            throw false;
        }

        while (query.next())
        {
            Cuma::Address::AddressBlock RetBlock;
            RetBlock.From.IP = query.value(0).toString();
            RetBlock.From.Port = query.value(1).toInt();
            RetBlock.To.IP = query.value(2).toString();
            RetBlock.To.Port = query.value(3).toInt();
            RetBlock.Direction = (Cuma::Address::BypassDirection(query.value(4).toInt()));
            RetBlock.BypassArray = (QJsonDocument::fromJson(query.value(5).toString().toUtf8()).array());
            RetBlock.PathCount = query.value(6).toInt();
            RetBlockList.append(RetBlock);
        }
        return RetBlockList;
    }
    catch (bool& b)
    {
        return RetBlockList;
    }
}

QString Cuma::DbAddress::DbAddressPathByFile::GetErrorString()
{
    return ErrorString;
}

QSqlError Cuma::DbAddress::DbAddressPathByFile::GetError()
{
    return Error;
}

bool Cuma::DbAddress::DbAddressPathByFile::isError()
{
    return (Error.type() != QSqlError::NoError);
}
