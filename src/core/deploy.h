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

    NBLog *logFile;

};

#endif // NBDEPLOY_H
