#ifndef NBSTATEDEPLOYING_H
#define NBSTATEDEPLOYING_H

#include "state.h"

class NBDeployThread;
class NBLog;
class QTimer;

class NBStateDeploying : public NBState
{
    Q_OBJECT
public:
    explicit NBStateDeploying(QObject *parent = 0);
    ~NBStateDeploying();

    void run();
    void shutUp();

public slots:
    void deployFinished();
    void timeout();

private:
    NBDeployThread *m_deploy;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_isError;

    NBLog *m_logWritter;
};

#endif // NBSTATEDEPLOYING_H
