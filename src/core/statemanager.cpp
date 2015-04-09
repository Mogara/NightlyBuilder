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

NBStateManager::NBStateManager(QObject *parent) : QObject(parent), m_currentState(NonState)
{
    for (State state = NonState; state <= Finished; ++state) {
        NBState *s = NBStateFactory::createState(state);
        if (s != NULL)
            s->setParent(this);

        m_stateMap[state] = s;
    }
}

NBStateManager::~NBStateManager()
{

}

void NBStateManager::start()
{
    m_currentState = Pulling;
    NBState *s = m_stateMap[m_currentState];

    if (s == NULL) {
        m_currentState = Error;
        emit error();
        return;
    }

    s->start();
}

void NBStateManager::oneFinished()
{
    ++m_currentState;
    if (m_currentState == Finished)
        emit finished();
    else {
        NBState *s = m_stateMap[m_currentState];
        if (s == NULL) {
            m_currentState = Error;
            emit error();
            return;
        }

        s->start();
    }
}

void NBStateManager::oneError()
{
    int retryLimit = 1;
    if (m_currentState == Pulling || m_currentState == Uploading)
        retryLimit = 5;

    NBState *s = qobject_cast<NBState *>(sender());
    if (s == NULL) {
        m_currentState = Error;
        emit error();
        return;
    }

    if (s->retryTimes() < retryLimit)
        s->retry();
    else {
        m_currentState = Error;
        emit error();
    }
}
