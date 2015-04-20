#include "operation.h"

#include <QTimer>

NBOperation::NBOperation(QObject *parent) : QObject(parent)
{
    m_operationTimer = new QTimer;
    m_operationTimer->setSingleShot(true);
    connect(m_operationTimer, &QTimer::timeout, this, &NBOperation::timeout);
}

NBOperation::~NBOperation()
{
    if (m_operationTimer != NULL) {
        disconnect(m_operationTimer, &QTimer::timeout, this, 0);
        delete m_operationTimer;
    }
}

void NBOperation::timeout()
{
    doTimeout();
}
