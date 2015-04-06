#ifndef NBSTATEPACKAGING_H
#define NBSTATEPACKAGING_H

#include "state.h"

class NBStatePackaging : public NBState
{
    Q_OBJECT
public:
    explicit NBStatePackaging(QObject *parent = 0);
    ~NBStatePackaging();

    void run();

signals:

public slots:
};

#endif // NBSTATEPACKAGING_H
