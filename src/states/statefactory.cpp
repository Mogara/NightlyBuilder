#include "statefactory.h"

#include "state.h"
#include "statecleaning.h"
#include "statedeploying.h"
#include "stategeneratingbotversioncpp.h"
#include "statemaking.h"
#include "statepackaging.h"
#include "statepulling.h"
#include "stateqmaking.h"
#include "stateuploading.h"


NBState *NBStateFactory::createState(NBStateManager::State state)
{
    switch (state) {
        case NBStateManager::Pulling:
            return new NBStatePulling;
        case NBStateManager::GeneratingBotVersionCpp:
            return new NBStateGeneratingBotVersionCpp;
        case NBStateManager::Cleaning:
            return new NBStateCleaning;
        case NBStateManager::QMaking:
            return new NBStateQMaking;
        case NBStateManager::Making:
            return new NBStateMaking;
        case NBStateManager::Deploying:
            return new NBStateDeploying;
        case NBStateManager::Packaging:
            return new NBStatePackaging;
        case NBStateManager::Uploading:
            return new NBStateUploading;

        default:
            return NULL;
    }

    return NULL;
}
