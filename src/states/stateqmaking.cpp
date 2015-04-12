#include "stateqmaking.h"
#include "globalconfig.h"

#include <QTimer>
#include <QDir>

NBStateQMaking::NBStateQMaking(QObject *parent) : NBState(parent), m_qmake(NULL), m_waitTimer(NULL), m_running(false), m_isError(false)
{

}

NBStateQMaking::~NBStateQMaking()
{

}


void NBStateQMaking::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;
    m_isError = false;

    if (m_waitTimer != NULL) {
        delete m_waitTimer;
        m_waitTimer = NULL;
    }

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
    m_qmake->setWorkingDirectory(GlobalConfig::BuildPath);
    m_qmake->setProgram("qmake");

    QDir buildDir(GlobalConfig::BuildPath);
    QString relativePath = buildDir.relativeFilePath(GlobalConfig::DeployPath);

    m_qmake->setArguments(QStringList() << "\"CONFIG+=buildbot\"" << relativePath);

    connect(m_qmake, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStateQMaking::processFinished);
    connect(m_qmake, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStateQMaking::processError);

    // start timer before the process start

    m_waitTimer = new QTimer();
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(600000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateQMaking::timeout);

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

    // log
    Q_UNUSED(stdOut); // temp solution
    Q_UNUSED(stdErr);

    m_running = false;

    if (!m_isError)
        emit finished();
}

void NBStateQMaking::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_qmake->state() != QProcess::NotRunning)
        m_qmake->kill();

    emit error();
}

void NBStateQMaking::shutUp()
{
    m_waitTimer->stop();
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
