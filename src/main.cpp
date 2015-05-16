#include "maindialog.h"
#include "global.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, a.applicationDirPath());

    NBSettingPtr = new GlobalConfig;
    QObject::connect(&a, &QApplication::aboutToQuit, NBSettingPtr, &GlobalConfig::deleteLater);

    NBSetting.init();

    NBMainDialog w;
    w.show();

    return a.exec();
}
