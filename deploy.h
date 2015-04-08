#ifndef NBDEPLOY_H
#define NBDEPLOY_H

#include <QThread>

class NBDeployThread : public QThread
{
    Q_OBJECT
public:
    NBDeployThread();
    ~NBDeployThread();

    void run();

};

#endif // NBDEPLOY_H
