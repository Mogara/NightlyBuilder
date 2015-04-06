#ifndef NBSTATEQMAKING_H
#define NBSTATEQMAKING_H

#include "state.h"

class NBStateQMaking : public NBState
{
    Q_OBJECT

public:
    explicit NBStateQMaking(QObject *parent = 0);
    ~NBStateQMaking();
};

#endif // NBSTATEQMAKING_H
