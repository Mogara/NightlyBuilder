#ifndef NBSTATEQMAKING_H
#define NBSTATEQMAKING_H

#include "state.h"

class NBStateQMaking : public NBState
{
    Q_OBJECT

public:
    explicit NBStatePulling(QObject *parent = 0);
    ~NBStateQMaking();
};

#endif // NBSTATEQMAKING_H
