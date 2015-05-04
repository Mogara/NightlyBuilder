#ifndef NBUPLOADTHREAD_H
#define NBUPLOADTHREAD_H

#include <QThread>
#include <QString>

class QNetworkAccessManager;
class QUrl;

class NBFtpUpload : public QObject
{
    Q_OBJECT

public:
    NBFtpUpload(QObject *parent = NULL);
    ~NBFtpUpload();

    void start();

signals:
    void finished();

private:
    QNetworkAccessManager *m_man;
    QUrl *m_url;
    QString m_filePath;
};


class NBUploadThread : public QThread
{
    Q_OBJECT

public:
    NBUploadThread();
    ~NBUploadThread();

    void run();

private slots:
    void uploadFinished();
};

#endif // NBUPLOADTHREAD_H
