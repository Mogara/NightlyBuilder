#include "maindialog.h"
#include "statemanager.h"
#include "global.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, a.applicationDirPath());

    NBSettingPtr = new NBSettings;
    QObject::connect(&a, &QApplication::aboutToQuit, NBSettingPtr, &NBSettings::deleteLater);

    NBGlobal_Settings.init();

    NBStateManagerPtr = new NBStateManager;
    QObject::connect(&a, &QApplication::aboutToQuit, NBStateManagerPtr, &NBStateManager::deleteLater);

    NBMainDialog w;
    w.show();

    return a.exec();
}
