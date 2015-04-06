#include "state.h"

NBState::NBState(QObject *parent) : QObject(parent), m_retryTimes(0)
{

}

NBState::~NBState()
{

}

void NBState::start()
{
    m_retryTimes = 0;
    run();
}

void NBState::retry()
{
    ++m_retryTimes;
    run();
}
