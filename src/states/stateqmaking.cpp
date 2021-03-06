#include "stateqmaking.h"
#include "global.h"
#include "log.h"

#include <QTimer>
#include <QDir>

NBStateQMaking::NBStateQMaking(QObject *parent) : NBState(parent), m_qmake(NULL), m_waitTimer(NULL), m_running(false), m_isError(false)
{

}

NBStateQMaking::~NBStateQMaking()
{
    if (m_waitTimer != NULL) {
        m_waitTimer->stop();
        delete m_waitTimer;
    }

    if (m_qmake != NULL) {
        disconnect(m_qmake, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_qmake, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_qmake->state() != QProcess::NotRunning)
            m_qmake->kill();
        if (m_qmake->state() == QProcess::NotRunning || m_qmake->waitForFinished()) {

        } else
            GlobalMethod::crash();

        m_qmake->deleteLater();
    }
}


void NBStateQMaking::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;
    m_isError = false;

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_qmake != NULL) {
        disconnect(m_qmake, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
        disconnect(m_qmake, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
        if (m_qmake->state() != QProcess::NotRunning)
            m_qmake->kill();
        if (m_qmake->state() == QProcess::NotRunning || m_qmake->waitForFinished()) {

        } else {
            // log
            m_isError = true;
            emit fatal();
            return;
        }
        delete m_qmake;
        m_qmake = NULL;
    }

    m_qmake = new QProcess;
    m_qmake->setWorkingDirectory(NBSettings::BuildPath);
    m_qmake->setProgram("qmake");

    QDir buildDir(NBSettings::BuildPath);
    QString relativePath = buildDir.relativeFilePath(NBSettings::ProjectPath);

    m_qmake->setArguments(QStringList() << "CONFIG+=buildbot" << relativePath);

    connect(m_qmake, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStateQMaking::processFinished);
    connect(m_qmake, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStateQMaking::processError);

    // start timer before the process start

    if (m_waitTimer == NULL)
        m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(600000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateQMaking::timeout);
    m_waitTimer->start();

    m_qmake->start();
}

void NBStateQMaking::processError(QProcess::ProcessError)  // designed to handle error, temporily no effect
{
    m_isError = true;
}

void NBStateQMaking::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
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
    logstdOut.openLogFile("QMake-StdOut");
    logstdOut.insertLog(stdOut);
    logstdOut.closeLogFile();
    NBLog logstdErr;
    logstdErr.openLogFile("QMake-StdErr");
    logstdErr.insertLog(stdErr);
    logstdErr.closeLogFile();

    m_running = false;

    if (!m_isError)
        emit finished();
}

void NBStateQMaking::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_qmake == NULL) {
        // what the hell?
        return;
    }

    if (m_qmake->state() != QProcess::NotRunning)
        m_qmake->kill();

    emit error();
}

void NBStateQMaking::shutUp()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_qmake == NULL) {
        // what the fuck??
        return;
    }

    disconnect(m_qmake, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, 0);
    disconnect(m_qmake, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, 0);
    if (m_qmake->state() != QProcess::NotRunning)
        m_qmake->kill();
    if (m_qmake->state() == QProcess::NotRunning || m_qmake->waitForFinished()) {

    } else {
        /*
        // log
        m_isError = true;
        emit fatal();
        return;
        */
    }
    m_qmake->deleteLater();
    m_qmake = NULL;

    emit stopped();
}
