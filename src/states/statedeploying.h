#ifndef NBSTATEDEPLOYING_H
#define NBSTATEDEPLOYING_H

#include "state.h"

class NBDeployThread;
class QTimer;

class NBStateDeploying : public NBState
{
    Q_OBJECT
public:
    explicit NBStateDeploying(QObject *parent = 0);
    ~NBStateDeploying();

    void run();

public slots:
    void deployFinished();
    void timeout();

private:
    NBDeployThread *m_deploy;
    QTimer *m_waitTimer;
    bool m_running;
    bool m_isError;
};

#endif // NBSTATEDEPLOYING_H
