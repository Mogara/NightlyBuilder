#include "log.h"
#include "global.h"

#include <QFile>
#ifdef NBLOG_THREAD_SUPPORT
#include <QThread>
#endif
#include <QDir>
#include <QDate>

NBLog::NBLog(QObject *parent) : QObject(parent), m_opened(false), m_logFile(NULL)
#ifdef NBLOG_THREAD_SUPPORT
  , m_logThread(NULL)
#endif
{

}

NBLog::~NBLog()
{
    if (m_logFile != NULL) {
#ifdef NBLOG_THREAD_SUPPORT
        m_logFile->moveToThread(QThread::currentThread());
#endif
        m_logFile->close();
        m_logFile->deleteLater();
    }

#ifdef NBLOG_THREAD_SUPPORT
    if (m_logThread != NULL) {
        if (m_logThread->isRunning())
            m_logThread->quit();
        if (!m_logThread->isRunning() || m_logThread->wait(10000UL)) {

        } else
            GlobalMethod::crash();

        m_logThread->deleteLater();
    }
#endif

}


bool NBLog::openLogFile(const QString &logName)
{
    if (m_logFile != NULL)
        closeLogFile();

    QDir d(GlobalConfig::LogPath);
    if (!d.exists())
        d.mkpath(GlobalConfig::LogPath);

    QString fileName = d.absoluteFilePath(logName + QDate::currentDate().toString("yyyyMMdd") + ".log");

    m_logFile = new QFile(fileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

#ifdef NBLOG_THREAD_SUPPORT
    if (m_logThread == NULL) {
        m_logThread = new QThread;
        m_logThread->start();
    }

    m_logFile->moveToThread(m_logThread);
#endif

    connect(this, &NBLog::writeLog, m_logFile, (qint64 (QIODevice::*)(const QByteArray &))(&QIODevice::write));
    return true;
}

void NBLog::closeLogFile()
{
    if (m_logFile == NULL)
        return;

#ifdef NBLOG_THREAD_SUPPORT
    if (m_logThread != NULL)
        m_logFile->moveToThread(QThread::currentThread());
#endif

    m_logFile->close();
    m_logFile->deleteLater();
    m_logFile = NULL;
}

void NBLog::insertLog(const QString &contents)
{
    QByteArray ba = contents.toUtf8();
    emit writeLog(ba);
}
