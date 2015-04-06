#include "maindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NBMainDialog w;
    w.show();

    return a.exec();
}
