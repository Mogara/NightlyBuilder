#include "statepulling.h"

#include <QProcess>
#include <QTimer>

NBStatePulling::NBStatePulling(QObject *parent) : NBState(parent), m_git(NULL), m_waitTimer(NULL), m_running(false)
{

}

NBStatePulling::~NBStatePulling()
{

}


void NBStatePulling::run()
{
    if (m_running)
        return;

    m_running = true;

    if (m_waitTimer != NULL) {
        delete m_waitTimer;
        m_waitTimer = NULL;
    }

    if (m_git != NULL) {
        m_git->kill();
        if (m_git->state() == QProcess::NotRunning || m_git->waitForFinished()) {

        } else {
            // log
            emit error();
            return;
        }
        delete m_git;
        m_git = NULL;
    }

    m_git = new QProcess;
    m_git->setProgram("git");
    m_git->setArguments(QStringList() << "pull");

    connect(m_git, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStatePulling::processFinished);
    connect(m_git, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStatePulling::processError);

    m_git->start();
}

void NBStatePulling::processError(QProcess::ProcessError)  // designed to handle error, temporily no effect
{

}

void NBStatePulling::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *p = qobject_cast<QProcess *>(sender());

    if (p == NULL) {
        // log
        emit error();
        return;
    }


    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = QString::fromLocal8Bit(p->readAllStandardError());
        Q_UNUSED(stdErr);
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

    emit finished();
}
