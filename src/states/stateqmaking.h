#ifndef NBSTATEQMAKING_H
#define NBSTATEQMAKING_H

#include "state.h"
#include <QProcess>

class QTimer;

class NBStateQMaking : public NBState
{
    Q_OBJECT

public:
    explicit NBStateQMaking(QObject *parent = 0);
    ~NBStateQMaking();

    void run();

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void timeout();

private:
    QProcess *m_qmake;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_isError;
};

#endif // NBSTATEQMAKING_H
