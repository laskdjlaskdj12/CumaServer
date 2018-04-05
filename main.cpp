#include <QCoreApplication>
#include <QTest>
#include "Test/spreadhandletest.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    SpreadHandleTest test;
    QTest::qExec(&test);

    return a.exec();
}
