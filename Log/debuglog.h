#ifndef DEBUGLOG_H
#define DEBUGLOG_H
#include <QObject>
#include <QDebug>

#define DEBUGLOG(X) DebugLog::ShowDebug(X, __LINE__, __FUNCTION__, __FILE__)

class DebugLog : public QObject
{
    Q_OBJECT
public:
    static inline void ShowDebug(const QString& str, unsigned int line, QString FunctionName, QString FileName)
    {
        qDebug() << "[Debug] : " + FileName + " : " + FunctionName + ":" + QString::number(line) + " : " + str;
    }
};

#endif // DEBUGLOG_H
