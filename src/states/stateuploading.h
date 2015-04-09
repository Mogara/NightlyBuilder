#ifndef NBSTATEUPLOADING_H
#define NBSTATEUPLOADING_H

#include "state.h"

class NBStateUploading : public NBState
{
    Q_OBJECT
public:
    explicit NBStateUploading(QObject *parent = 0);
    ~NBStateUploading();

    void run();

signals:

public slots:
};

#endif // NBSTATEUPLOADING_H
