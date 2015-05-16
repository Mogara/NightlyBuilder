#ifndef NBUPLOAD_H
#define NBUPLOAD_H

#include <QString>
#include <QNetworkReply>

class QNetworkAccessManager;
class QUrl;
class NBCopyThread;

class NBUpload : public QObject
{
    Q_OBJECT

public:
    NBUpload(QObject *parent = 0);
    ~NBUpload();

signals:
    void finished();
    void error();
    void stopped();

public slots:
    void stop();
    void start();
    void uploadFinished();
    void networkError(QNetworkReply::NetworkError);

private:
    NBCopyThread *m_thread; // for copy & paste
    QNetworkAccessManager *m_network; // for FTP upload
    QNetworkReply *m_currentReply;
    QUrl m_url;
    QString m_filePath;

    bool m_isFtp;
    bool m_succeed;
    bool m_running;
};

#endif // NBUPLOADTHREAD_H
