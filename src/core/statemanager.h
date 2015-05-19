#ifndef NBSTATEMANAGER_H
#define NBSTATEMANAGER_H

#include <QObject>
#include <QMap>

class NBState;


// A State Manager running in main thread.
// All the states are also running in main thread, but they work with a process or an another thread.


class NBStateManager : public QObject
{
    Q_OBJECT

public:
    enum State {
        NonState,                  // State with nothing to do, it is usually the begining of the whole process.
        Pulling,                   // State with a running process "git pull" pulling the newest code from Github
        GeneratingBotVersionCpp,   // State which generating bot_version.cpp changing the version of the generated program
        Cleaning,                  // State which cleans the building dir
        QMaking,                   // State with a running process "qmake" generating Makefile
        Making,                    // State with a running process "make" building the whole project to a single runnable EXE
        Deploying,                 // State with a thread that copy files and folders so that computers without Qt can run it
        Packaging,                 // State with a running process "7z" packing the files and folders to a 7z package
        Uploading,                 // State with a thread that copy the packaged 7z file to server
        Finished,                  // State with nothing to do, but it informs the whole process succeeded.

        Error = -1                 // State with error occurred.
    };

    explicit NBStateManager(QObject *parent = 0);
    ~NBStateManager();

signals:
    void finished();
    void error();
    void stopped();
    void state_changed(State);

public slots:
    void start();
    void stop();

    void oneFinished();
    void oneError();
    void oneStopped();
    void oneFatal();

private:
    QMap<State, NBState *> m_stateMap;
    static const QMap<State, QString> m_stateOutput1LogfileMap;
    static const QMap<State, QString> m_stateOutput2LogfileMap;
    static const QMap<State, QString> m_stateOutput3LogfileMap;
    State m_currentState;
    bool m_stopping;

    void setCurrentState(State s);
};

extern NBStateManager *NBStateManagerPtr;

#define NBGlobal_StateManager (*NBStateManagerPtr)

NBStateManager::State operator++(NBStateManager::State &arg);

NBStateManager::State operator++(NBStateManager::State &arg, int);

NBStateManager::State operator+(const NBStateManager::State &arg1, int arg2);


#endif // NBSTATEMANAGER_H
