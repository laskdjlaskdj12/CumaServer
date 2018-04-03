#ifndef ERRORLOG_H
#define ERRORLOG_H
#include <QObject>
#include <QDebug>

#define ERRLOG(X) Cuma::ErrorLog(X, __FILE__, __FUNCTION__, __LINE__)

namespace Cuma
{
class ErrorLog: public QObject
{
    Q_OBJECT
public:
    ErrorLog(QString string, QString function, int Line)
    {
        qDebug() << "[Error] : " + function + "() " + "LINE : " + QString::number(Line) + " Reason : " + string;
    }
    ErrorLog(QString string, QString FileName, QString function, int Line)
    {
        qDebug() << "[Error] : " + FileName + " : " + function + "() " + "LINE : " + QString::number(Line) + " Reason : " + string;
    }
};
}
#endif // ERRORLOG_H
