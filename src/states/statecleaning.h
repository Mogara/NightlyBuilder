#ifndef NBSTATECLEANING_H
#define NBSTATECLEANING_H

#include "state.h"

class NBStateCleaning : public NBState
{
    Q_OBJECT
public:
    explicit NBStateCleaning(QObject *parent = 0);
    ~NBStateCleaning();

    void run();
    void shutUp();

signals:

public slots:
};

#endif // NBSTATECLEANING_H
