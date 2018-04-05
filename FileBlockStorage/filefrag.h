#ifndef FILEFRAG_H
#define FILEFRAG_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QDebug>

#include "Block/FileBlock/blockstruct.h"
namespace Cuma
{
namespace FileBlockStorage
{

class FileFragDir : public QObject
{
    Q_OBJECT
public:
    explicit FileFragDir(QObject* parent = nullptr);
    ~FileFragDir();

    /*
     * FileFrag의 저장위치를 리턴
     * */
    virtual QUrl GetFileSaveLocation();

    /*
     * FileFrag의 저장위치를 설정
     * */
    virtual void SetFileSaveLocation(QUrl Location);


    /*
     * FileFrag를 저장
     * */
    virtual bool SaveFileBlock(Cuma::FileBlock::FileBlock FileBlock);

    /*
     * FileFrag를 삭제
     * */
    virtual bool RemoveFile(QString FileName, int index);

    /*
     * FileFrag를 로드
     * */
    virtual Cuma::FileBlock::FileBlock GetFileBlock(QString FileName, int index);
    virtual Cuma::FileBlock::FileBlock GetFileBlock(QString FilePath);

    /*
     * Error가 있을경우 ErrorStr를 리턴
     * */
    virtual QString GetErrorStr();

private:
    QFile FileObj;

    QDir CurrentDir;

    QString RootDir;

    QString ErrorStr;
};

}

}
#endif // FILEFRAG_H
