#include "statemanager.h"
#include "statefactory.h"
#include "state.h"


NBStateManager::State operator++(NBStateManager::State &arg)
{
    arg = static_cast<NBStateManager::State>(static_cast<int>(arg) + 1);
    return arg;
}

NBStateManager::State operator++(NBStateManager::State &arg, int /*rightOperatorHandler*/)
{
    NBStateManager::State s = arg;
    arg = static_cast<NBStateManager::State>(static_cast<int>(arg) + 1);
    return s;
}

NBStateManager::State operator+(const NBStateManager::State &arg1, int arg2)
{
    int _arg1 = arg1;
    _arg1 += arg2;
    return static_cast<NBStateManager::State>(_arg1);
}

NBStateManager::NBStateManager(QObject *parent) : QObject(parent), m_currentState(NonState), m_stopping(false)
{
    for (State state = NonState; state <= Finished; ++state) {
        NBState *s = NBStateFactory::createState(state);
        if (s != NULL) {
            s->setParent(this);
            connect(s, &NBState::finished, this, &NBStateManager::oneFinished);
            connect(s, &NBState::error, this, &NBStateManager::oneError);
            connect(s, &NBState::stopped, this, &NBStateManager::oneStopped);
            connect(s, &NBState::fatal, this, &NBStateManager::oneFatal);
        }

        m_stateMap[state] = s;
    }
}

NBStateManager::~NBStateManager()
{

}

void NBStateManager::start()
{
    m_stopping = false;

    setCurrentState(Pulling);
    NBState *s = m_stateMap[m_currentState];

    if (s == NULL) {
        setCurrentState(Error);
        emit error();
        return;
    }

    s->start();
}

void NBStateManager::stop()
{
    NBState *s = m_stateMap[m_currentState];
    if (s != NULL) {
        s->stop();
        m_stopping = true;
    } else {
        // Stop with nothing on going??? What is the ghost???
        oneStopped();
    }
}

void NBStateManager::oneStopped()
{
    m_stopping = false;
    emit stopped();
}

void NBStateManager::oneFinished()
{
    if (m_stopping) {
        oneStopped();
        return;
    }

    setCurrentState(m_currentState + 1);
    if (m_currentState == Finished)
        emit finished();
    else {
        NBState *s = m_stateMap[m_currentState];
        if (s == NULL) {
            setCurrentState(Error);
            emit error();
            return;
        }

        s->start();
    }
}

void NBStateManager::oneError()
{
    if (m_stopping) {
        oneStopped();
        return;
    }

    int retryLimit = 1;
    if (m_currentState == Pulling || m_currentState == Uploading)
        retryLimit = 5;

    NBState *s = qobject_cast<NBState *>(sender());
    if (s == NULL) {
        setCurrentState(Error);
        emit error();
        return;
    }

    if (s->retryTimes() < retryLimit)
        s->retry();
    else {
        setCurrentState(Error);
        emit error();
    }
}

void NBStateManager::oneFatal()
{
    if (m_stopping) {
        oneStopped();
        return;
    }

    setCurrentState(Error);
    emit error();
}

void NBStateManager::setCurrentState(State s)
{
    State former = m_currentState;
    if (former == s)
        return;

    m_currentState = s;
    emit state_changed(m_currentState);
}
