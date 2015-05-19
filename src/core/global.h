#ifndef NBGLOBAL_H
#define NBGLOBAL_H

#include <QString>
#include <QSettings>

class NBSettings : public QSettings
{
public:
    NBSettings();
    void init();

    static QString ProjectPath;
    static QString BuildPath;
    static QString QtPath;
    static QString DeployPath;
    static QString FtpPath;
    static QString LogPath;

    static QString FtpUserName;
    static QString FtpPassword;

};

namespace GlobalMethod
{
    void crash();
}

extern NBSettings *NBSettingPtr;

#define NBGlobal_Settings (*NBSettingPtr)

#endif // GLOBAL_H

