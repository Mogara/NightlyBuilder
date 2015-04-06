#ifndef NBSTATEPULLING_H
#define NBSTATEPULLING_H

#include "state.h"

class NBStatePulling : public NBState
{
    Q_OBJECT
public:
    explicit NBStatePulling(QObject *parent = 0);
    ~NBStatePulling();

    void run();

signals:

public slots:
};

#endif // NBSTATEPULLING_H
