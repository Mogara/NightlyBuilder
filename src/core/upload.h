#ifndef NBUPLOAD_H
#define NBUPLOAD_H

#include <QThread>


class NBUploadThread : public QThread
{
    Q_OBJECT

public:
    NBUploadThread();
    ~NBUploadThread();

    void run();
};

#endif // NBUPLOAD_H
