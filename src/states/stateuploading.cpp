#include "stateuploading.h"
#include "upload.h"

#include <QTimer>

NBStateUploading::NBStateUploading(QObject *parent) : NBState(parent)
{

}

NBStateUploading::~NBStateUploading()
{

}


void NBStateUploading::run()
{
    if (m_running) {
        emit error();
        return;
    }

    m_running = true;

    if (m_waitTimer != NULL) {
        delete m_waitTimer;
        m_waitTimer = NULL;
    }

    if (m_upload != NULL) {
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

    m_waitTimer = new QTimer;
    m_waitTimer->setSingleShot(true);
    m_waitTimer->setInterval(60000);
    connect(m_waitTimer, &QTimer::timeout, this, &NBStateUploading::timeout);

    m_upload->start();
}

void NBStateUploading::uploadFinished()
{
    m_running = false;
    emit finished();

    m_upload->deleteLater();
    m_upload = NULL;
}

void NBStateUploading::timeout()
{
    //m_running = false;

    if (m_upload->isRunning())
        m_upload->terminate();

    emit error();
}
