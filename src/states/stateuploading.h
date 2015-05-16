#ifndef NBSTATEUPLOADING_H
#define NBSTATEUPLOADING_H

#include "state.h"

class NBUpload;
class QTimer;

class NBStateUploading : public NBState
{
    Q_OBJECT
public:
    explicit NBStateUploading(QObject *parent = 0);
    ~NBStateUploading();

    void run();
    void shutUp();

public slots:
    void uploadFinished();
    void uploadError();
    void timeout();

private:
    NBUpload *m_upload;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_stopping;
};

#endif // NBSTATEUPLOADING_H
