#ifndef NBSTATEMAKING_H
#define NBSTATEMAKING_H

#include "state.h"

class NBStateMaking : public NBState
{
    Q_OBJECT
public:
    explicit NBStateMaking(QObject *parent = 0);
    ~NBStateMaking();

signals:

public slots:
};

#endif // NBSTATEMAKING_H
