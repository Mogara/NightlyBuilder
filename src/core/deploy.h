#ifndef NBDEPLOY_H
#define NBDEPLOY_H

#include <QThread>

class NBLog;

class NBDeployThread : public QThread
{
    Q_OBJECT
public:
    NBDeployThread();
    ~NBDeployThread();

    void run();

    bool succeed;
};

#endif // NBDEPLOY_H
