#include "filefrag.h"

Cuma::FileBlockStorage::FileFragDir::FileFragDir(QObject* parent) : QObject(parent)
{

}

Cuma::FileBlockStorage::FileFragDir::~FileFragDir()
{
}

QUrl Cuma::FileBlockStorage::FileFragDir::GetFileSaveLocation()
{
    return RootDir;
}

void Cuma::FileBlockStorage::FileFragDir::SetFileSaveLocation(QUrl Location)
{
    RootDir = Location.toString();
    CurrentDir.setPath(Location.toString());
}

bool Cuma::FileBlockStorage::FileFragDir::SaveFileBlock(Cuma::FileBlock::FileBlock FileBlock)
{
    QString FileName = RootDir + "/" + FileBlock.FileName + QString::number(FileBlock.Index) + ".Cuma";

    if (FileObj.isOpen())
    {
        FileObj.close();
    }

    FileObj.setFileName(FileName);
    FileObj.open(QFile::WriteOnly);

    if (FileObj.isOpen() == false)
    {
        qDebug() << " 파일 오픈을 실패했습니다. : Reason : " + FileObj.errorString();
        ErrorStr = FileObj.errorString();
        FileObj.close();
        return false;
    }

    FileObj.write(FileBlock.FileSource);
    FileObj.close();
    return true;
}

bool Cuma::FileBlockStorage::FileFragDir::RemoveFile(QString FileName, int index)
{
    QString TempFileName = FileName + QString::number(index) + ".Cuma";

    QFile file(TempFileName);

    if (file.remove() == false)
    {
        qDebug() << "File remove fail : " + file.errorString();
    }


    if (CurrentDir.exists(TempFileName) == true)
    {
        return CurrentDir.remove(TempFileName);
    }
    return true;
}

Cuma::FileBlock::FileBlock Cuma::FileBlockStorage::FileFragDir::GetFileBlock(QString FileName, int index)
{
    Cuma::FileBlock::FileBlock RetBlock;

    QString FILENAME = FileName + QString::number(index) + ".Cuma";
    QString FILENAMEPATH = RootDir + "/" + FILENAME;

    QDir dir;
    dir.setPath(RootDir);

    QStringList EntityList = dir.entryList();

    for (int i = 0; i < EntityList.count(); i++)
    {
        QString EntityFileName = EntityList.at(i);

        if (EntityFileName == FILENAME)
        {
            RetBlock.FileName = FileName;
            break;
        }
    }

    if (RetBlock.FileName.isEmpty())
    {
        return RetBlock;
    }

    if (FileObj.isOpen())
    {
        FileObj.close();
    }

    FileObj.setFileName(FILENAMEPATH);
    FileObj.open(QFile::ReadOnly);

    if (FileObj.isOpen() == false)
    {
        ErrorStr = FileObj.errorString();
        FileObj.close();
        return RetBlock;
    }

    RetBlock.FileName = FileName;
    RetBlock.FileSource = FileObj.readAll();
    RetBlock.Index = index;
    RetBlock.FileSize = RetBlock.FileSource.size();

    FileObj.close();
    return RetBlock;
}

Cuma::FileBlock::FileBlock Cuma::FileBlockStorage::FileFragDir::GetFileBlock(QString FilePath)
{
    Cuma::FileBlock::FileBlock RetBlock;

    if (FileObj.isOpen())
    {
        FileObj.close();
    }

    FileObj.setFileName(FilePath);
    FileObj.open(QFile::ReadOnly);

    if (FileObj.isOpen() == false)
    {
        ErrorStr = FileObj.errorString();
        FileObj.close();
        return RetBlock;
    }

    QString FileName = FileObj.fileName();
    unsigned int FileNameEnd = FileName.indexOf(".Cuma") - 1;
    unsigned int FileNameStart  = 0;

    for (unsigned int i = 0; i < FileName.size(); i++)
    {
        int ResIndex = FileName.indexOf("/", FileNameStart + 1);
        if (ResIndex == -1)
        {
            break;
        }

        FileNameStart = ResIndex;
    }
    unsigned int NumberIndex = 0;
    for (unsigned int i = FileNameStart; i < FileNameEnd + 1; i++)
    {
        if (FileName.at(i).isNumber() == true)
        {
            NumberIndex = i;
            break;
        }
    }
    QString TempFileName = FileName.mid(FileNameStart, NumberIndex - FileNameStart - 1);
    int TempIndex = FileName.mid(NumberIndex, FileNameEnd).toInt();

    if (TempIndex == -1)
    {
        FileObj.close();
        return RetBlock;
    }

    //파일의 정보를 입력
    RetBlock.FileName = TempFileName;
    RetBlock.FileSource = FileObj.readAll();
    RetBlock.Index = TempIndex;
    RetBlock.FileSize = RetBlock.FileSource.size();

    FileObj.close();
    return RetBlock;

}

QString Cuma::FileBlockStorage::FileFragDir::GetErrorStr()
{
    return ErrorStr;
}
