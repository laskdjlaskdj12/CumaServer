#ifndef BLOCKSTRUCT_H
#define BLOCKSTRUCT_H

#include <QByteArray>
#include <QString>
#include <QJsonObject>

namespace Cuma 
{

namespace FileBlock
{

typedef struct FileBlock
{
    FileBlock()
    {
        Version = 1.0f;
    }
    float Version;
    QByteArray FileSource;
    QString FileName;
    unsigned int Index;
    unsigned int FileFragCount;
    unsigned int FileSize;
    
}FileBlock;

typedef struct FileFragInfo
{
    QString FileName;
    unsigned int Index;
    QString FilePid;

}FileFragInfo;

class Serlize
{

public:
    static QJsonObject FileBlockToJson (Cuma::FileBlock::FileBlock FileBlock)
    {
        QJsonObject Ret;

        Ret["Version"] = FileBlock.Version;
        Ret["FileSource"] = QString(FileBlock.FileSource);
        Ret["FileName"] = FileBlock.FileName;
        Ret["Index"] = static_cast<int> (FileBlock.Index) ;
        Ret["FileFragCount"] = static_cast<int>(FileBlock.FileFragCount);
        Ret["FileSize"] = static_cast<int>(FileBlock.FileSize);
        return Ret;
    }

    static Cuma::FileBlock::FileBlock JsonToFileBlock (QJsonObject obj)
    {
        FileBlock Block;

        Block.Version = obj["Version"].toDouble();
        Block.FileName = obj["FileName"].toString();
        Block.FileSource = obj["FileSource"].toString().toUtf8();
        Block.Index = obj["Index"].toInt();
        Block.FileFragCount = obj["FileFragCount"].toDouble();
        Block.FileSize = obj["FileSize"].toDouble();

        return Block;
    }

    static QJsonObject FileInfoToJson(Cuma::FileBlock::FileFragInfo info)
    {
        QJsonObject obj;
        obj["FileName"] = QString (info.FileName);
        obj["Index"] = static_cast<int>(info.Index);
        obj["FilePid"] = QString(info.FilePid);
        return obj;
    }

    static Cuma::FileBlock::FileFragInfo JsonToFileInfo (QJsonObject obj)
    {
        FileFragInfo Block;

        Block.FileName = obj["FileName"].toString();
        Block.Index = obj["Index"].toInt();
        Block.FilePid = obj["FilePid"].toString();

        return Block;
    }
};

}
}

#endif // BLOCKSTRUCT_H
