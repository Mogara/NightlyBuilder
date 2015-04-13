#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

class QLineEdit;
class QVBoxLayout;

class NBMainDialog : public QDialog
{
    Q_OBJECT

public:
    NBMainDialog(QWidget *parent = 0);
    ~NBMainDialog();

private:
    QVBoxLayout *m_layout;
    QLineEdit *m_projectPathEdit;
    QLineEdit *m_buildPathEdit;
    QLineEdit *m_qtPathEdit;
    QLineEdit *m_deployPathEdit;
    QLineEdit *m_ftpPathEdit;
    QHash<QPushButton *, QLineEdit *> m_buttonEditPairs;

    void addPathLine(const QString &name, QLineEdit *&edit);

private slots:
    void showFileDialog();
    void saveSettings();
};

#endif // MAINDIALOG_H
