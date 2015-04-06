#ifndef NBSTATEQMAKING_H
#define NBSTATEQMAKING_H

#include "state.h"

class NBStateQMaking : public NBState
{
    Q_OBJECT

public:
    explicit NBStateQMaking(QObject *parent = 0);
    ~NBStateQMaking();

    void run();

};

#endif // NBSTATEQMAKING_H
