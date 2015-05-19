#include "upload.h"
#include "global.h"
#include "copythread.h"

#include <QDate>
#include <QFileInfo>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>

#ifdef USE_FSTREAM
#include <fstream>
using std::fstream;
using std::string;
using std::ios_base;
#endif

NBUpload::NBUpload(QObject *parent) : QObject(parent), m_thread(NULL), m_network(NULL), m_currentReply(NULL)
{

}

NBUpload::~NBUpload()
{
    if (m_thread != NULL) {
        disconnect(m_thread, &QThread::finished, this, 0);
        if (m_thread->isRunning())
            m_thread->terminate();
        if (!m_thread->isRunning() || m_thread->wait()) {

        } else
            GlobalMethod::crash();

        m_thread->deleteLater();
    }

    if (m_network != NULL) {
        disconnect(m_currentReply, &QNetworkReply::finished, this, 0);
        disconnect(m_currentReply, ((void (QNetworkReply::*)(QNetworkReply::NetworkError))(&QNetworkReply::error)), this, 0);
        // I don't know whether this operation is safe or not.
        delete m_currentReply;
        delete m_network;
    }
}

void NBUpload::start()
{
    m_succeed = true;
    m_running = true;
    if (NBSettings::FtpPath.toLower().startsWith("ftp://")) {
        m_isFtp = true;
        if (m_network != NULL) {
            if (m_currentReply != NULL) {
                disconnect(m_currentReply, &QNetworkReply::finished, this, 0);
                disconnect(m_currentReply, ((void (QNetworkReply::*)(QNetworkReply::NetworkError))(&QNetworkReply::error)), this, 0);
                m_currentReply->abort();
                m_currentReply->deleteLater();
                m_currentReply = NULL;
            }
        } else {
            m_network = new QNetworkAccessManager;
        }

        QString filename = QDir(NBSettings::ProjectPath).dirName() + "-" + QDate::currentDate().toString("yyyyMMdd") +
#if defined(Q_OS_WIN)
            ".7z"
#elif defined(Q_OS_LINUX)
#if defined(LINUX_X86)
            "-x86"
#elif defined(LINUX_X64)
            "-x64"
#else
            "-xxx"
#endif
            + ".tar.gz"
#endif
        ;

        QString uploadUrl = NBSettings::FtpPath;
        if (!uploadUrl.endsWith("/"))
            uploadUrl.append("/");

        uploadUrl.append(filename);
        m_url.setUrl(uploadUrl);
        m_url.setUserName(NBSettings::FtpUserName);
        m_url.setPassword(NBSettings::FtpPassword);

        m_filePath = QDir(NBSettings::DeployPath).absoluteFilePath(filename);

#ifndef USE_FSTREAM
        QFile *f = new QFile(m_filePath, this);
        f->open(QIODevice::ReadOnly);
#else
        QFileInfo file(m_filePath);
        string path = QDir::toNativeSeparators(file.canonicalFilePath()).toStdString();
        fstream fs;
        fs.open(path, ios_base::in | ios_base::binary);

        fs.seekg(0, ios_base::end);
        fstream::pos_type size = fs.tellg();
        fs.seekg(0, ios_base::beg);

        QByteArray f(size, 0);
        fs.read(f.data(), size);
#endif
        m_currentReply = m_network->put(QNetworkRequest(m_url), f);
        connect(m_currentReply, ((void (QNetworkReply::*)(QNetworkReply::NetworkError))(&QNetworkReply::error)), this, &NBUpload::networkError);
        connect(m_currentReply, &QNetworkReply::finished, this, &NBUpload::uploadFinished);
    } else {
        m_isFtp = false;

        if (m_thread != NULL) {
            disconnect(m_thread, &QThread::finished, this, 0);
            if (m_thread->isRunning())
                m_thread->terminate();
            if (!m_thread->isRunning() || m_thread->wait()) {

            } else
                GlobalMethod::crash();

            m_thread->deleteLater();
        }

        m_thread = new NBCopyThread;
        connect(m_thread, &QThread::finished, this, &NBUpload::uploadFinished);
        m_thread->start();
    }
}

void NBUpload::networkError(QNetworkReply::NetworkError)
{
    m_succeed = false;
}

void NBUpload::stop()
{
    if (m_running) {
        m_succeed = false;
        if (m_isFtp) {
            disconnect(m_currentReply, &QNetworkReply::finished, this, 0);
            disconnect(m_currentReply, ((void (QNetworkReply::*)(QNetworkReply::NetworkError))(&QNetworkReply::error)), this, 0);
            m_currentReply->abort();
            m_currentReply->deleteLater();
            m_currentReply = NULL;
        } else {
            disconnect(m_thread, &QThread::finished, this, 0);
            if (m_thread->isRunning())
                m_thread->terminate();
            if (!m_thread->isRunning() || m_thread->wait()) {

            } else
                GlobalMethod::crash();

            m_thread->deleteLater();
            m_thread = NULL;
        }
    }
    m_running = false;
    emit stopped();
}

void NBUpload::uploadFinished()
{
    m_running = false;
    if (m_isFtp) {
        disconnect(m_currentReply, &QNetworkReply::finished, this, 0);
        disconnect(m_currentReply, ((void (QNetworkReply::*)(QNetworkReply::NetworkError))(&QNetworkReply::error)), this, 0);
        m_currentReply->deleteLater();
        m_currentReply = NULL;
    } else {
        m_succeed = m_thread->succeed;
        disconnect(m_thread, &QThread::finished, this, 0);
        m_thread->deleteLater();
        m_thread = NULL;
    }

    if (m_succeed)
        emit finished();
    else
        emit error();
}
