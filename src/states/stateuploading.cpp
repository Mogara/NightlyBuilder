#include "stateuploading.h"
#include "upload.h"
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
        disconnect(m_upload, &NBUpload::stopped, this, 0);
        disconnect(m_upload, &NBUpload::error, this, 0);
        disconnect(m_upload, &NBUpload::finished, this, 0);
        delete m_upload;
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
        disconnect(m_upload, &NBUpload::stopped, this, 0);
        disconnect(m_upload, &NBUpload::error, this, 0);
        disconnect(m_upload, &NBUpload::finished, this, 0);
        delete m_upload;
    }

    m_upload = new NBUpload;

    connect(m_upload, &NBUpload::finished, this, &NBStateUploading::uploadFinished);
    connect(m_upload, &NBUpload::error, this, &NBStateUploading::uploadError);
    connect(m_upload, &NBUpload::stopped, this, &NBStateUploading::uploadFinished);

    // start timer before the thread start

    if (m_waitTimer == NULL)
        m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(7200000);
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

    m_stopping = true;
    m_upload->stop();
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

    if (m_stopping)
        emit stopped();
    else
        emit finished();

    disconnect(m_upload, &NBUpload::stopped, this, 0);
    disconnect(m_upload, &NBUpload::error, this, 0);
    disconnect(m_upload, &NBUpload::finished, this, 0);
    m_upload->deleteLater();
    m_upload = NULL;
}

void NBStateUploading::timeout()
{
    m_running = false;

    if (m_upload == NULL) {
        // what the hell?
        return;
    }

    disconnect(m_upload, &NBUpload::stopped, this, 0);
    disconnect(m_upload, &NBUpload::error, this, 0);
    disconnect(m_upload, &NBUpload::finished, this, 0);
    m_upload->stop();

    emit error();
}

void NBStateUploading::uploadError()
{
    if (m_waitTimer != NULL)
        m_waitTimer->stop();

    if (m_upload == NULL) {
        // what the fuck??
        return;
    }

    m_running = false;
    if (m_stopping)
        emit stopped();
    else
        emit error();

    disconnect(m_upload, &NBUpload::stopped, this, 0);
    disconnect(m_upload, &NBUpload::error, this, 0);
    disconnect(m_upload, &NBUpload::finished, this, 0);
    m_upload->deleteLater();
    m_upload = NULL;
}
