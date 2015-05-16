#ifndef NBCOPYTHREAD_H
#define NBCOPYTHREAD_H


#include <QThread>

class NBCopyThread : public QThread
{
    Q_OBJECT

public:
    NBCopyThread();
    ~NBCopyThread();

    void run();

    bool succeed;
};

#endif
