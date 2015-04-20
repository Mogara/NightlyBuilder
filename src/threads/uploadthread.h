#ifndef NBUPLOADTHREAD_H
#define NBUPLOADTHREAD_H

#include <QThread>


class NBUploadThread : public QThread
{
    Q_OBJECT

public:
    NBUploadThread();
    ~NBUploadThread();

    void run();
};

#endif // NBUPLOADTHREAD_H
