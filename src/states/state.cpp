#include "state.h"

NBState::NBState(QObject *parent) : QObject(parent), m_stopping(false), m_retryTimes(0)
{

}

NBState::~NBState()
{

}

void NBState::start()
{
    m_retryTimes = 0;
    m_stopping = false;
    run();
}

void NBState::retry()
{
    ++m_retryTimes;
    m_stopping = false;
    run();
}

void NBState::stop()
{
    m_stopping = true;
    shutUp();
}
