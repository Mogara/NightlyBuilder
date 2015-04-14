#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

namespace GlobalConfig
{
    extern const QString DefaultBuildPath;
    extern const QString DefaultDeployPath;

    extern QString ProjectPath;
    extern QString BuildPath;
    extern QString QtPath;
    extern QString DeployPath;
    extern QString FtpPath;
    extern QString LogPath;
}

namespace GlobalMethod
{
    void crash();
}

#endif // GLOBAL_H

