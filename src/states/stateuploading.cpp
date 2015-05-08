#include "stateuploading.h"
#include "uploadthread.h"
#include "global.h"

#include <QTimer>

NBStateUploading::NBStateUploading(QObject *parent) : NBState(parent), m_upload(NULL), m_waitTimer(NULL), m_running(false)
{

}

NBStateUploading::~NBStateUploading()
{
    if (m_waitTimer != NULL) {
        m_waitTimer->stop();
        delete m_waitTimer;
    }

    if (m_upload != NULL) {
        disconnect(m_upload, &QThread::finished, this, 0);
        if (m_upload->isRunning())
            m_upload->terminate();
        if (!m_upload->isRunning() || m_upload->wait()) {

        } else
            GlobalMethod::crash();

        m_upload->deleteLater();
    }
}


void NBStateUploading::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;

    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_upload != NULL) {
        disconnect(m_upload, &QThread::finished, this, 0);
        if (m_upload->isRunning())
            m_upload->terminate();
        if (!m_upload->isRunning() || m_upload->wait()) {

        } else {
            emit fatal();
            return;
        }
        delete m_upload;
        m_upload = NULL;
    }

    m_upload = new NBUploadThread;

    connect(m_upload, &QThread::finished, this, &NBStateUploading::uploadFinished);

    // start timer before the thread start

    if (m_waitTimer == NULL)
        m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(1800000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateUploading::timeout);
    m_waitTimer->start();

    m_upload->start();
}

void NBStateUploading::shutUp()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_upload == NULL) {
        // not running?
        return;
    }

    disconnect(m_upload, &QThread::finished, this, 0);
    if (m_upload->isRunning())
        m_upload->terminate();
    if (!m_upload->isRunning() || m_upload->wait()) {

    } else {
    /*
        m_isError = true;
        emit fatal();
        return;
    */
        // we ignore this error.
    }
    m_upload->deleteLater();
    m_upload = NULL;

    emit stopped();
}

void NBStateUploading::uploadFinished()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_upload == NULL) {
        // what the fuck??
        return;
    }

    m_running = false;
    emit finished();

    m_upload->deleteLater();
    m_upload = NULL;
}

void NBStateUploading::timeout()
{
    //m_running = false;

    if (m_upload == NULL) {
        // what the hell?
        return;
    }

    if (m_upload->isRunning())
        m_upload->terminate();

    emit error();
}
