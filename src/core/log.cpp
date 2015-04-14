#include "log.h"
#include "global.h"

#include <QFile>
#include <QThread>
#include <QDir>
#include <QDate>

NBLog::NBLog(QObject *parent) : QObject(parent), m_opened(false), m_logFile(NULL), m_logThread(NULL)
{

}

NBLog::~NBLog()
{
    if (m_logFile != NULL) {
        m_logFile->moveToThread(QThread::currentThread());
        m_logFile->close();
        m_logFile->deleteLater();
    }

    if (m_logThread != NULL) {
        if (m_logThread->isRunning())
            m_logThread->quit();
        if (!m_logThread->isRunning() || m_logThread->wait(10000UL)) {

        } else
            GlobalMethod::crash();

        m_logThread->deleteLater();
    }
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

    if (m_logThread == NULL) {
        m_logThread = new QThread;
        m_logThread->start();
    }

    m_logFile->moveToThread(m_logThread);

    connect(this, &NBLog::writeLog, m_logFile, (qint64 (QIODevice::*)(const QByteArray &))(&QIODevice::write));
    return true;
}

void NBLog::closeLogFile()
{
    if (m_logFile == NULL)
        return;

    if (m_logThread != NULL)
        m_logFile->moveToThread(QThread::currentThread());

    m_logFile->close();
    m_logFile->deleteLater();
    m_logFile = NULL;
}

void NBLog::insertLog(const QString &contents)
{
    QByteArray ba = contents.toUtf8();
    emit writeLog(ba);
}
