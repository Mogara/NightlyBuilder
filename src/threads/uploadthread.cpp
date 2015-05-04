#include "uploadthread.h"
#include "global.h"

#include <QDir>
#include <QFile>
#include <QDate>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>

NBFtpUpload::NBFtpUpload(QObject *parent) : QObject(parent)
{
    m_man = new QNetworkAccessManager(this);
    QString filename = QDir(GlobalConfig::ProjectPath).dirName() + "-" + QDate::currentDate().toString("yyyyMMdd") + ".7z";
    QString uploadUrl = GlobalConfig::FtpPath;
    if (!uploadUrl.endsWith("/"))
        uploadUrl.append("/");

    uploadUrl.append(filename);

    m_url = new QUrl(uploadUrl);

    m_filePath = QDir(GlobalConfig::DeployPath).absoluteFilePath(filename);
}

NBFtpUpload::~NBFtpUpload()
{
    delete m_url;
}

void NBFtpUpload::setUserName(const QString &username)
{
    m_url->setUserName(username);
}

void NBFtpUpload::setPassWord(const QString &password)
{
    m_url->setPassword(password);
}

void NBFtpUpload::start()
{
    QFile *f = new QFile(m_filePath, this);
    f->open(QIODevice::ReadOnly);

    QNetworkReply *r = m_man->put(QNetworkRequest(*m_url), f);
    connect(r, &QNetworkReply::finished, this, &NBFtpUpload::finished);
}

NBUploadThread::NBUploadThread()
{

}

NBUploadThread::~NBUploadThread()
{

}

void NBUploadThread::run()
{
    if (!GlobalConfig::FtpPath.toLower().startsWith("ftp://")) {
        QDir ftpDir(GlobalConfig::FtpPath);
        QList<QFileInfo> f = ftpDir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
        if (f.length() > 14)
            QFile::remove(f.first().absoluteFilePath());

        QString fileName = QDir(GlobalConfig::ProjectPath).dirName() + "-" + QDate::currentDate().toString("yyyyMMdd") + ".7z";

        QDir dplyDir(GlobalConfig::DeployPath);

        QFile::copy(dplyDir.absoluteFilePath(fileName), ftpDir.absoluteFilePath(fileName));
    } else {
        NBFtpUpload *upload = new NBFtpUpload(this);
        upload->setUserName("Mogara");
        upload->setPassWord("Mogara233");

        connect(upload, &NBFtpUpload::finished, this, &NBUploadThread::uploadFinished);

        upload->start();

        exec();
    }
}

void NBUploadThread::uploadFinished()
{
    exit(0);
}
