#ifndef NBLOG_H
#define NBLOG_H

//#define NBLOG_THREAD_SUPPORT

#include <QObject>

class QIODevice;
#ifdef NBLOG_THREAD_SUPPORT
class QThread;
#endif

class NBLog : public QObject
{
    Q_OBJECT
public:
    explicit NBLog(QObject *parent = 0);
    ~NBLog();

    bool openLogFile(const QString &logName);
    void closeLogFile();

signals:
    void writeLog(const QByteArray &contents);

public slots:
    void insertLog(const QString &contents);

private:
    bool m_opened;
    QIODevice *m_logFile;
#ifdef NBLOG_THREAD_SUPPORT
    QThread *m_logThread;
#endif

};

#endif // NBLOG_H
