#ifndef NBOPERATION_H
#define NBOPERATION_H

#include <QObject>

class QTimer;

class NBOperation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timer READ timer WRITE setTimer)

public:
    virtual void operationRun() = 0;
    virtual void operationShutUp() = 0;

public:
    inline void setTimer(int timer) { m_timer = timer; }
    inline int timer() { return m_timer; }


signals:
    void operationTimeout();                    // Design for the timer, this timer is started in NBOperation but not the state.
    void operationFinished();                   // Indicate the Process/Thread is finished.
    void operationError();                      // Indicate the Process/Thread is running into error.
    void operationOutput1(const QByteArray &);  // Used for real-time outputing, 1st channel
    void operationOutput2(const QByteArray &);  // Used for real-time outputing, 2nd channel
    void operationOutput3(const QByteArray &);  // Used for real-time outputing, 3rd channel

public slots:
    void timeout();

protected:
    virtual void doTimeout() = 0;

protected:
    NBOperation(QObject *parent = NULL);
    virtual ~NBOperation();

protected:
    QTimer *m_operationTimer;
    int m_timer;
};

#endif
