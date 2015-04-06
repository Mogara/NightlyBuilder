#ifndef NBSTATE_H
#define NBSTATE_H

#include <QObject>

class NBState : public QObject
{
    Q_OBJECT
public:
    explicit NBState(QObject *parent = 0);
    ~NBState();

    virtual void run() = 0;

    inline int retryTimes() const { return m_retryTimes; }

signals:
    void finished();
    void error();

private:
    int m_retryTimes;

public slots:
    void retry();
    void start();
};

#endif // NBSTATE_H
