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

    /*
     * 테이블이 있는지 체크
     * */
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

    query.prepare("INSERT INTO `FileFragAddress`(`FromIP`, `FromPort`, `ToIp`, `ToPort`, `Direction`, `BypassArray`, `PathCount`, `FileName`, `Pid`, )"
                  " VALUES (:FileName , :From , :To , :Bypasscount , :FileBlockPid);");

    query.bindValue(":FromIP", AddressBlock.From.IP );
    query.bindValue(":FromPort", AddressBlock.From.IP );
    query.bindValue(":ToIp", AddressBlock.From.IP );
    query.bindValue(":ToPort", AddressBlock.From.IP );
    query.bindValue(":Direction", AddressBlock.From.IP );
    query.bindValue(":FromPort", AddressBlock.From.IP );
    query.bindValue(":FromPort", AddressBlock.From.IP );
    query.bindValue(":FromPort", AddressBlock.From.IP );


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

    query.prepare("DELETE FROM `FileFragAddress` WHERE `From` = (:ip) ");

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

    query.prepare("DELETE FROM `FileFragAddress` WHERE `To` = (:ip) ");

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

    query.prepare("DELETE FROM `FileFragAddress` WHERE `FileName` = (:FileName) AND `FileBlockPid` = (:FileBlockPid) ");

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
            Error = AddressDb.lastError();
            ErrorString = "AddressDb is not open : " + AddressDb.lastError().text();
            throw false;
        }

        QSqlQuery query(AddressDb);

        query.prepare("SELECT * FROM `FileFragAddress` WHERE `FileName` = (:FileName) AND `FileBlockPid` = (:FileBlockPid) ");

        query.bindValue(":FileName", FileName);
        query.bindValue(":FileBlockPid", Pid);

        if (query.exec() == false)
        {
            qDebug() << "[Error] : Add Query is fail";
            Error = AddressDb.lastError();
            ErrorString = "Add Query is fail: " + query.lastError().text();
            throw false;
        }

        int FromIndex = query.record().indexOf("From");
        int ToIndex = query.record().indexOf("To");
        int BypassCountIndex = query.record().indexOf("BypassCount");
        query.next();

        Cuma::Address::AddressBlock RetBlock;

        RetBlock.From.IP = query.value(FromIndex).toString();
        RetBlock.To.IP = query.value(ToIndex).toString();
        RetBlock.PathCount = query.value(BypassCountIndex).toInt();
        return RetBlock;

    }

    catch (bool& b)
    {
        Cuma::Address::AddressBlock EmptyBlock;
        return EmptyBlock;
    }
}

QVector<Cuma::Address::AddressBlock> Cuma::DbAddress::DbAddressPathByFile::GetAddress(QString FileName)
{
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

        int FromIndex = query.record().indexOf("From");
        int ToIndex = query.record().indexOf("To");
        int BypassCountIndex = query.record().indexOf("BypassCount");

        QVector<Cuma::Address::AddressBlock> RetBlocks;

        while (query.next())
        {
            Cuma::Address::AddressBlock RetBlock;
            RetBlock.From.IP = query.value(FromIndex).toString();
            RetBlock.To.IP = query.value(ToIndex).toString();
            RetBlock.PathCount = query.value(BypassCountIndex).toInt();
            RetBlocks.append(RetBlock);

        }

        return RetBlocks;

    }
    catch (bool& b)
    {
        QVector<Cuma::Address::AddressBlock>  EmptyBlock;
        return EmptyBlock;
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
