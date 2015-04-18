#include "maindialog.h"
#include "global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NBSetting.init();

    NBMainDialog w;
    w.show();

    return a.exec();
}
