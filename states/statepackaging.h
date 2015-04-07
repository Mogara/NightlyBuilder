#ifndef NBSTATEPACKAGING_H
#define NBSTATEPACKAGING_H

#include "state.h"
#include <QProcess>

class QTimer;

class NBStatePackaging : public NBState
{
    Q_OBJECT
public:
    explicit NBStatePackaging(QObject *parent = 0);
    ~NBStatePackaging();

    void run();

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void timeout();

private:
    QProcess *m_7z;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_isError;
};

#endif // NBSTATEPACKAGING_H
