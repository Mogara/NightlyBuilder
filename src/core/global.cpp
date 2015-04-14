

#include "global.h"

#include <QMessageBox>

namespace GlobalConfig
{
    const QString DefaultBuildPath = "../build-%1-%2";
    const QString DefaultDeployPath = "../%1-nightly-%2";

    QString ProjectPath = "";
    QString BuildPath = "";
    QString QtPath = "";
    QString DeployPath = "";
    QString FtpPath = "";
    QString LogPath = "";
}

void GlobalMethod::crash()
{
    // unhandled error occurred!!!
    // temporary solution: force the program to terminate!!!
    QMessageBox::critical(NULL, QObject::tr("Nightly Builder"), QObject::tr("Fatal error occurred. This program is going to terminate."));
    std::terminate();
}
