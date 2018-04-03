#include <QCoreApplication>
#include <QTest>
#include "servertest.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    ServerTest test;
    QTest::qExec(&test);

    return a.exec();
}
