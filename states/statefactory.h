#ifndef NBSTATEFACTORY_H
#define NBSTATEFACTORY_H

#include "statemanager.h"

class NBState;

namespace NBStateFactory
{
    NBState *createState(NBStateManager::State state);
}


#endif // NBSTATEFACTORY_H
