#include "log.h"
#include "global.h"
#ifndef USE_FSTREAM
#include <QFile>
#else
#include <fstream>
using std::fstream;
using std::ios_base;
#endif

#include <QDir>
#include <QDate>

NBLog::NBLog(QObject *parent) : QObject(parent), m_opened(false), m_logFile(NULL)
{

}

NBLog::~NBLog()
{
    if (m_logFile != NULL) {
#ifndef USE_FSTREAM
        m_logFile->close();
        m_logFile->deleteLater();
#else
        m_logFile->close();
        delete m_logFile;
#endif
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

#ifndef USE_FSTREAM
    m_logFile = new QFile(fileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
#else
    m_logFile = new fstream;
    m_logFile->open(fileName.toLocal8Bit().constData(), ios_base::out | ios_base::trunc);
#endif

    return true;
}

void NBLog::closeLogFile()
{
    if (m_logFile == NULL)
        return;

    m_logFile->close();
#ifndef USE_FSTREAM
    m_logFile->deleteLater();
#else
    delete m_logFile;
#endif
    m_logFile = NULL;
}

void NBLog::insertLog(const QString &contents)
{
    QByteArray ba = contents.toUtf8();
#ifndef USE_FSTREAM
    m_logFile->write(ba);
#else
    m_logFile->write(ba.constData(), ba.length());
#endif
}
