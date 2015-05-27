#ifndef NBLOG_H
#define NBLOG_H

#include <QObject>
#ifndef USE_FSTREAM
class QIODevice;
#else
#include <fstream>
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
#ifndef USE_FSTREAM
    QIODevice *m_logFile;
#else
    std::fstream *m_logFile;
#endif

};

#endif // NBLOG_H
