#include <QCoreApplication>
#include <QTest>
#include "Test/serverspreadtest.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    ServerSpreadTest test;
    QTest::qExec(&test);

    return a.exec();
}
