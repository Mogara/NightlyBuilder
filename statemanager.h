#ifndef NBSTATEMANAGER_H
#define NBSTATEMANAGER_H

#include <QObject>

class NBStateManager : public QObject
{
    Q_OBJECT

    enum States {
        NonState,                  // State with nothing to do, it is usually the begining of the whole process.
        Pulling,                   // State with a running process "git pull" pulling the newest code from Github
        GeneratingBotVersionCpp,   // State with a thread that generating bot_version.cpp changing the version of the whole program
        QMaking,                   // State with a running process "qmake" generating Makefile
        Making,                    // State with a running process "make" building the whole project to a single runnable EXE
        Deploying,                 // State with a thread that copy files and folders so that computers without Qt can run it
        Packaging,                 // State with a running process "7z" packing the files and folders to a 7z package
        Uploading,                 // State with a thread that copy the packaged 7z file to server
        Finished,                  // State with nothing to do, but it informs the whole process succeeded.

        Error = -1                 // State with error occurred.
    };

public:
    explicit NBStateManager(QObject *parent = 0);
    ~NBStateManager();

signals:

public slots:
};

#endif // NBSTATEMANAGER_H
