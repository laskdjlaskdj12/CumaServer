#ifndef DBADDRESSTEST_H
#define DBADDRESSTEST_H

#include <QObject>
#include <QTest>

#include <QDir>

#include "Block/AddressBlock/addressblock.h"
#include "DbAddressPath/dbaddresspathbyfile.h"

class DbAddressTest: public QObject
{
    Q_OBJECT

public:
    DbAddressTest()
    {
    }

    ~DbAddressTest()
    {
    }

protected:
    Cuma::Address::AddressBlock makeTestBlockAddress()
    {
        Cuma::Address::AddressBlock ReturnAddressBlock;

        ReturnAddressBlock.From.IP = "127.0.0.1";
        ReturnAddressBlock.From.Port = 7070;
        ReturnAddressBlock.To.IP = "127.0.0.1";
        ReturnAddressBlock.To.Port = 7071;
        ReturnAddressBlock.PathCount = 0;
        ReturnAddressBlock.Direction = Cuma::Address::BypassDirection::Req;

        return ReturnAddressBlock;
    }

private slots:

    void initTestCase()
    {
        if (Dir.exists("TestDb") == false)
        {
            Dir.mkdir("TestDb");
        }

        Dir.cd("TestDb");

        QDir::setCurrent(Dir.absolutePath());
    }

    void cleanupTestCase()
    {
        Dir.cdUp();

        QDir::setCurrent(Dir.absolutePath());

        Dir.cd("TestDb");

        Dir.removeRecursively();
    }

    //    void AddDbAddressPathByFile()
    //    {
    //        Cuma::DbAddress::DbAddressPathByFile AddressDB;

    //        Cuma::Address::AddressBlock AddressBlock = makeTestBlockAddress();

    //        QVERIFY (AddressDB.Add(AddressBlock, "Test.txt", "01ABCED"));

    //        QVERIFY (AddressDB.GetAddress("Test.txt", "01ABCED").To.IP.isEmpty() == false);
    //    }

    void RemoveAddressFile()
    {
        Cuma::DbAddress::DbAddressPathByFile AddressDB;

        Cuma::Address::AddressBlock AddressBlock = makeTestBlockAddress();

        QVERIFY (AddressDB.Add(AddressBlock, "Test.txt", "01ABCED"));

        QVERIFY (AddressDB.GetAddress("Test.txt", "01ABCED").To.IP.isEmpty() == false);

        QVERIFY (AddressDB.RemoveFromFileAddress("Test.txt", "01ABCED"));

        QVERIFY (AddressDB.GetAddress("Test.txt", "01ABCED").To.IP.isEmpty() == true);
    }


private:
    QDir Dir;
    QString RootDirectory;

};

#endif // DBADDRESSTEST_H
