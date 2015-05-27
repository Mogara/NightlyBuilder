#ifndef NBLOG_H
#define NBLOG_H

#include <QObject>

class QIODevice;

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

};

#endif // NBLOG_H
