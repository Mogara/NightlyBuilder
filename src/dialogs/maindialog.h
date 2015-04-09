#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

class NBMainDialog : public QDialog
{
    Q_OBJECT

public:
    NBMainDialog(QWidget *parent = 0);
    ~NBMainDialog();
};

#endif // MAINDIALOG_H
