#ifndef NBGLOBAL_H
#define NBGLOBAL_H

#include <QString>
#include <QSettings>

class GlobalConfig : public QSettings
{
public:
    GlobalConfig();
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

extern GlobalConfig *NBSettingPtr;

#define NBSetting (*NBSettingPtr)

#endif // GLOBAL_H

