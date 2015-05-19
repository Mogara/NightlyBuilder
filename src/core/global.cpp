#include "global.h"

#include <QMessageBox>

QString NBSettings::ProjectPath;
QString NBSettings::BuildPath;
QString NBSettings::QtPath;
QString NBSettings::DeployPath;
QString NBSettings::FtpPath;
QString NBSettings::LogPath;
QString NBSettings::FtpUserName = "anonymous";
QString NBSettings::FtpPassword = "User@Mogara.org";

NBSettings *NBSettingPtr;

NBSettings::NBSettings() : QSettings("MogaraNightlyBuilder", QSettings::IniFormat)
{

}

void NBSettings::init()
{
#define LOAD_STRING_SETTING(name) name = value(#name).toString()

    LOAD_STRING_SETTING(ProjectPath);
    LOAD_STRING_SETTING(BuildPath);
    LOAD_STRING_SETTING(QtPath);
    LOAD_STRING_SETTING(DeployPath);
    LOAD_STRING_SETTING(FtpPath);
    LOAD_STRING_SETTING(LogPath);
    LOAD_STRING_SETTING(FtpUserName);
    LOAD_STRING_SETTING(FtpPassword);

#undef LOAD_STRING_SETTING
}

void GlobalMethod::crash()
{
    // unhandled error occurred!!!
    // temporary solution: force the program to terminate!!!
    QMessageBox::critical(NULL, QObject::tr("Nightly Builder"), QObject::tr("Fatal error occurred. This program is going to terminate."));
    std::terminate();
}
