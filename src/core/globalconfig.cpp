

#include "globalconfig.h"

namespace GlobalConfig {
    const QString DefaultBuildPath = "../build-%1-%2";
    const QString DefaultDeployPath = "../%1-nightly-%2";

    QString ProjectPath = "";
    QString BuildPath = "";
    QString QtPath = "";
    QString DeployPath = "";
}
