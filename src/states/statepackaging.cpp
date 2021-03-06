#include "statepackaging.h"
#include "global.h"
#include "log.h"

#include <QTimer>
#include <QDir>
#include <QDate>

NBStatePackaging::NBStatePackaging(QObject *parent) : NBState(parent), m_7z(NULL), m_waitTimer(NULL), m_running(false), m_isError(false)
{

}

NBStatePackaging::~NBStatePackaging()
{
    if (m_waitTimer != NULL) {
        m_waitTimer->stop();
        delete m_waitTimer;
    }

    if (m_7z != NULL) {
        disconnect(m_7z, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_7z, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_7z->state() != QProcess::NotRunning)
            m_7z->kill();
        if (m_7z->state() == QProcess::NotRunning || m_7z->waitForFinished()) {

        } else
            GlobalMethod::crash();

        m_7z->deleteLater();
    }
}


void NBStatePackaging::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;
    m_isError = false;

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_7z != NULL) {
        disconnect(m_7z, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_7z, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_7z->state() != QProcess::NotRunning)
            m_7z->kill();
        if (m_7z->state() == QProcess::NotRunning || m_7z->waitForFinished()) {

        } else {
            // log
            m_isError = true;
            emit fatal();
            return;
        }
        delete m_7z;
        m_7z = NULL;
    }

    m_7z = new QProcess;

    QDir dplyDir(NBSettings::DeployPath);
    QString folderName = QDate::currentDate().toString("yyyyMMdd");

    QDir projDir(NBSettings::ProjectPath);
    QString projName = projDir.dirName();

    m_7z->setWorkingDirectory(dplyDir.absolutePath());
#if defined(Q_OS_WIN)
    m_7z->setProgram("7z");
    m_7z->setArguments(QStringList() << "a" << (projName + "-" + folderName + ".7z") << folderName);
#elif defined(Q_OS_LINUX)
    m_7z->setProgram("tar");
    m_7z->setArguments(QStringList() << "-czf" << (projName + "-" + folderName +
#if defined(LINUX_X86)
        "-x86"
#elif defined(LINUX_X64)
        "-x64"
#else
        "-xxx"
#endif
    + ".tar.gz") << folderName);
#endif

    connect(m_7z, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStatePackaging::processFinished);
    connect(m_7z, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStatePackaging::processError);

    // start timer before the process start

    if (m_waitTimer == NULL)
        m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(1800000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStatePackaging::timeout);
    m_waitTimer->start();

    m_7z->start();
}

void NBStatePackaging::processError(QProcess::ProcessError)  // designed to handle error, temporily no effect
{
    m_isError = true;
}

void NBStatePackaging::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *p = qobject_cast<QProcess *>(sender());

    if (p == NULL) {
        // log
        m_isError = true;
        emit fatal();
        return;
    }

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = QString::fromLocal8Bit(p->readAllStandardError());
        Q_UNUSED(stdErr);
        m_isError = true;
        m_running = false;
        //log
        emit error();
        return;
    }

    QString stdOut = QString::fromLocal8Bit(p->readAllStandardOutput());
    QString stdErr = QString::fromLocal8Bit(p->readAllStandardError());

    NBLog logstdOut;
    logstdOut.openLogFile("7z-StdOut");
    logstdOut.insertLog(stdOut);
    logstdOut.closeLogFile();
    NBLog logstdErr;
    logstdErr.openLogFile("7z-StdErr");
    logstdErr.insertLog(stdErr);
    logstdErr.closeLogFile();

    m_running = false;

    if (!m_isError)
        emit finished();
}

void NBStatePackaging::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_7z == NULL) {
        // what the hell?
        return;
    }

    if (m_7z->state() != QProcess::NotRunning)
        m_7z->kill();

    emit error();
}

void NBStatePackaging::shutUp()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_7z == NULL) {
        // what the fuck??
        return;
    }

    disconnect(m_7z, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
    disconnect(m_7z, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
    if (m_7z->state() != QProcess::NotRunning)
        m_7z->kill();
    if (m_7z->state() == QProcess::NotRunning || m_7z->waitForFinished()) {

    } else {
        /*
        // log
        m_isError = true;
        emit fatal();
        return;
        */
    }
    m_7z->deleteLater();
    m_7z = NULL;

    emit stopped();
}
