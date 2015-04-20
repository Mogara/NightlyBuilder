#ifndef NBDEPLOYTHREAD_H
#define NBDEPLOYTHREAD_H

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

#endif // NBDEPLOYTHREAD_H
