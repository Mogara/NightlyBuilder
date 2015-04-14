#include "upload.h"
#include "global.h"

#include <QDir>
#include <QFile>
#include <QDate>

NBUploadThread::NBUploadThread()
{

}

NBUploadThread::~NBUploadThread()
{

}

void NBUploadThread::run()
{
    QDir ftpDir(GlobalConfig::FtpPath);
    QList<QFileInfo> f = ftpDir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
    if (f.length() > 14)
        QFile::remove(f.first().absoluteFilePath());

    QString fileName = QDir(GlobalConfig::ProjectPath).dirName() + "-" + QDate::currentDate().toString("yyyyMMdd") + ".7z";

    QDir dplyDir(GlobalConfig::DeployPath);

    QFile::copy(dplyDir.absoluteFilePath(fileName), ftpDir.absoluteFilePath(fileName));
}
