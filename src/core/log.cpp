#include "log.h"

#include <QFile>
#include <QThread>

#include <QMessageBox>

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

        } else {
            // unhandled error occurred!!!
            // temporary solution: force the program to crash!!!
            QMessageBox::critical(NULL, tr("Nightly Builder"), tr("Fatal error occurred. This program is going to crash."));
            int &a = *((int *)(NULL));
            a = 1;
        }

        m_logThread->deleteLater();
    }
}


bool NBLog::openLogFile(const QString &logName)
{
    if (m_logFile != NULL)
        closeLogFile();

    m_logFile = new QFile(logName);
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