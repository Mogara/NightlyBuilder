#include "log.h"
#include "global.h"

#include <QFile>
#include <QDir>
#include <QDate>

NBLog::NBLog(QObject *parent) : QObject(parent), m_opened(false), m_logFile(NULL)
{

}

NBLog::~NBLog()
{
    if (m_logFile != NULL) {
        m_logFile->close();
        m_logFile->deleteLater();
    }
}


bool NBLog::openLogFile(const QString &logName)
{
    if (m_logFile != NULL)
        closeLogFile();

    QDir d(NBSettings::LogPath);
    if (!d.exists())
        d.mkpath(NBSettings::LogPath);

    QString fileName = d.absoluteFilePath(logName + QDate::currentDate().toString("yyyyMMdd") + ".log");

    m_logFile = new QFile(fileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    connect(this, &NBLog::writeLog, m_logFile, (qint64 (QIODevice::*)(const QByteArray &))(&QIODevice::write));
    return true;
}

void NBLog::closeLogFile()
{
    if (m_logFile == NULL)
        return;

    m_logFile->close();
    m_logFile->deleteLater();
    m_logFile = NULL;
}

void NBLog::insertLog(const QString &contents)
{
    QByteArray ba = contents.toUtf8();
    emit writeLog(ba);
}
