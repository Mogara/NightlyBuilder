#ifndef NBSTATEGENERATINGBOTVERSIONCPP_H
#define NBSTATEGENERATINGBOTVERSIONCPP_H

#include "state.h"

class NBStateGeneratingBotVersionCpp : public NBState
{
    Q_OBJECT
public:
    explicit NBStateGeneratingBotVersionCpp(QObject *parent = 0);
    ~NBStateGeneratingBotVersionCpp();

    void run();
    void shutUp();

signals:

public slots:
};

#endif // NBSTATEGENERATINGBOTVERSIONCPP_H
