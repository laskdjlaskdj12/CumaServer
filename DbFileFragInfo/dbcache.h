#ifndef DBCACHE_H
#define DBCACHE_H

#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QUrl>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QCryptographicHash>

#include "Block/FileBlock/blockstruct.h"

#define  DEBUG
namespace Cuma
{

namespace DbFileFrag
{

/*
 * 데이터베이스 할당은 filefrag.h에서 QSqlDatabase를 만들고 여기에 임포트 하는 방식
 * QSqlDatabase 소유권은 이 클래스가 가짐
 * */
class DbFileFragInfo : public QObject
{
    Q_OBJECT
public:
    DbFileFragInfo(QObject* parent = nullptr);

    DbFileFragInfo(QSqlDatabase Database);

    ~DbFileFragInfo();

    /*
     * 가지고 있는 FileFrag 의 이름들만 나열
     * */
    virtual QVector<QString> GetAllFileList();

    /*
     * 파일 Frag가 저장된 위치를 DB에 저장
     * */
    virtual bool AddFileFrag(Cuma::FileBlock::FileFragInfo FileBlock, QUrl Path);

    /*
     *  파일 Frag 저장된 주소를 찾기
     * */
    virtual QVector<QUrl> SearchFile(QString FileName);

    virtual QUrl SearchFile(Cuma::FileBlock::FileFragInfo block);

    /*
     *  파일 Frag 정보 검색
     * */
    virtual Cuma::FileBlock::FileFragInfo SearchFileInfo(QString Pid);

    virtual QVector<Cuma::FileBlock::FileFragInfo> SearchFileInfo(QString FileName, unsigned int FileIndex);

    /*
     * 저장된 파일 블록의 인덱스를 지움
     * */
    virtual bool RemoveFile(QString FileName);

    virtual bool RemoveFile(Cuma::FileBlock::FileFragInfo block);

    /*
     * sql 에러가 발견이 됬을때 에러가 있는지 체크
     * */
    virtual QSqlError GetError();

    /*
     * sql 에러의 GetErrorString()을 리턴
     * */
    virtual QString GetErrorString();

#ifdef DEBUG
    inline bool isFileFragInfoExsist(Cuma::FileBlock::FileFragInfo info)
    {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM `FileFragCache` WHERE `FileBlockPid` = (:FBlockPid)");

        query.bindValue(":FBlockPid", info.FilePid);

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

        return false;
    }

#endif

protected:
    QSqlDatabase db;

    QSqlError Error;

    QString ErrorStr;
};

}

}

#endif // DBCACHE_H
