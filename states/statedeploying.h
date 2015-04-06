#ifndef NBSTATEDEPLOYING_H
#define NBSTATEDEPLOYING_H

#include "state.h"

class NBStateDeploying : public NBState
{
    Q_OBJECT
public:
    explicit NBStateDeploying(QObject *parent = 0);
    ~NBStateDeploying();

signals:

public slots:
};

#endif // NBSTATEDEPLOYING_H
