#include "accessclientfirewall.h"

Cuma::NetworkConfig::AccessClientFirewall::AccessClientFirewall(QObject* parent) : QObject(parent), DBOpen(false)
{
    if (QSqlDatabase::contains("FireWall"))
    {
        AddressListDB = QSqlDatabase::database("FireWall");
    }
    else
    {
        AddressListDB = QSqlDatabase::addDatabase("QSQLITE", "FireWall");
        AddressListDB.setDatabaseName("FireWallList.db");
    }

    try
    {
        if (AddressListDB.open() == false)
        {
            /*
            * 파일이 오픈이 안되서 false를 리턴함
            * */
            throw false;
        }


        /*
         * 테이블이 있는지 체크
         * */

        QSqlQuery FireWallTable = AddressListDB.exec("SELECT tbl_name AS temp FROM sqlite_master");

        FireWallTable.next();

        if (FireWallTable.value(0).toString() == "FireWallWhiteList")
        {
            DBOpen = true;
            return;
        }

        /*
        * FireWall의 테이블을 만듬
        * */

        QSqlQuery VerifyAddTable = AddressListDB.exec("CREATE TABLE `FireWallWhiteList` ("
                                   "`IP`	TEXT,"
                                   "`Port`	INTEGER,"
                                   "`Access`	BLOB);");

        /*
             * 외부에 어떤 이유로 테이블 생성이 안될때
             * */
        if (AddressListDB.lastError().isValid())
        {
            ErrorStr = AddressListDB.lastError().databaseText();
            throw false;
        }
    }
    catch (bool open)
    {
        DBOpen = open;
    }
}

Cuma::NetworkConfig::AccessClientFirewall::~AccessClientFirewall()
{
    AddressListDB.close();
}

bool Cuma::NetworkConfig::AccessClientFirewall::Add(QString ip, unsigned int port)
{
    QSqlQuery QueryResult(AddressListDB);
    QueryResult.prepare("INSERT INTO `FireWallWhiteList`(`IP`,`Port`,`Access`) VALUES ( :IP, :Port, 1);");
    QueryResult.bindValue(":IP", ip);
    QueryResult.bindValue(":Port", port);

    if (QueryResult.exec() == false)
    {
        ErrorStr = QueryResult.lastError().databaseText();
        return false;
    }
    return true;
}

bool Cuma::NetworkConfig::AccessClientFirewall::Remove(QString ip, unsigned int port)
{
    QSqlQuery QueryResult(AddressListDB);
    QueryResult.prepare("DELETE FROM `FireWallWhiteList` WHERE `IP` = (:IP) AND `Port` = (:Port)");
    QueryResult.bindValue(":IP", ip);
    QueryResult.bindValue(":Port", static_cast<int>(port));

    if (QueryResult.exec() == false)
    {
        ErrorStr = QueryResult.lastError().databaseText();
        return false;
    }
    return true;
}

bool Cuma::NetworkConfig::AccessClientFirewall::isOpen()
{
    return DBOpen;
}

QString Cuma::NetworkConfig::AccessClientFirewall::GetErrorMessage()
{
    return ErrorStr;
}

bool Cuma::NetworkConfig::AccessClientFirewall::Search(QString ip)
{
    QSqlQuery QueryResult(AddressListDB);
    QueryResult.prepare("SELECT `Access` FROM `FireWallWhiteList` WHERE `IP` = :IP ");
    QueryResult.bindValue(":IP", ip);

    if (QueryResult.exec() == false)
    {
        ErrorStr = QueryResult.lastError().databaseText();
        return false;
    }

    QueryResult.last();
    if (QueryResult.at() + 1 > 0)
    {
        return true;
    }

    else
    {
        ErrorStr = "Can't find Any of WhiteList";
        return false;
    }
}
