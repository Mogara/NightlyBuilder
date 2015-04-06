#ifndef NBSTATE_H
#define NBSTATE_H

#include <QObject>

class NBState : public QObject
{
    Q_OBJECT
public:
    explicit NBState(QObject *parent = 0);
    ~NBState();

signals:

public slots:
};

#endif // NBSTATE_H
