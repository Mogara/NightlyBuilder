#ifndef NBSTATEMAKING_H
#define NBSTATEMAKING_H

#include "state.h"
#include <QProcess>

class QTimer;

class NBStateMaking : public NBState
{
    Q_OBJECT
public:
    explicit NBStateMaking(QObject *parent = 0);
    ~NBStateMaking();

    void run();

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void timeout();

private:
    QProcess *m_make;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_isError;
};

#endif // NBSTATEMAKING_H
