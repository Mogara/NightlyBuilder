#include "statemaking.h"
#include "globalconfig.h"

#include <QTimer>


NBStateMaking::NBStateMaking(QObject *parent) : NBState(parent), m_make(NULL), m_waitTimer(NULL), m_running(false), m_isError(false)
{

}

NBStateMaking::~NBStateMaking()
{

}


void NBStateMaking::run()
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

    if (m_make != NULL) {
        if (m_make->state() != QProcess::NotRunning)
            m_make->kill();
        if (m_make->state() == QProcess::NotRunning || m_make->waitForFinished()) {

        } else {
            // log
            m_isError = true;
            emit fatal();
            return;
        }
        delete m_make;
        m_make = NULL;
    }

    m_make = new QProcess;
    m_make->setWorkingDirectory(GlobalConfig::BuildPath);
    m_make->setProgram("mingw32-make"); // consider using GlobalConfig instead
    m_make->setArguments(QStringList() << "-fMakeFile.Release");

    connect(m_make, (void (QProcess::*)(int, QProcess::ExitStatus))(&QProcess::finished), this, &NBStateMaking::processFinished);
    connect(m_make, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &NBStateMaking::processError);

    // start timer before the process start

    m_waitTimer = new QTimer();
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(1800000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateMaking::timeout);

    m_make->start();
}

void NBStateMaking::processError(QProcess::ProcessError)  // designed to handle error, temporily no effect
{
    m_isError = true;
}

void NBStateMaking::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
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

void NBStateMaking::timeout()
{
    m_isError = true;
    //m_running = false;

    if (m_make->state() != QProcess::NotRunning)
        m_make->kill();

    emit error();
}
