#ifndef NBMAINDIALOG_H
#define NBMAINDIALOG_H

#include <QDialog>
#include "statemanager.h"

class QLineEdit;
class QVBoxLayout;
class QFormLayout;
class QCloseEvent;
class QLabel;

class NBMainDialog : public QDialog
{
    Q_OBJECT

public:
    NBMainDialog(QWidget *parent = 0);
    ~NBMainDialog();

private:
    QVBoxLayout *m_layout;
    QFormLayout *m_pathLineLayout;

    QPushButton *m_applyBtn;
    QPushButton *m_runBtn;

    QLineEdit *m_projectPathEdit;
    QLineEdit *m_buildPathEdit;
    QLineEdit *m_qtPathEdit;
    QLineEdit *m_deployPathEdit;
    QLineEdit *m_ftpPathEdit;
    QLineEdit *m_logPathEdit;
    QHash<QPushButton *, QLineEdit *> m_buttonEditPairs;

    void addPathLine(const QString &name, QLineEdit *&edit, const QString &settingsKey = QString());

private slots:
    void showFileDialog();
    void saveSettings();

    void startOrStopRunning();
    void startCompiling();

    void runFinishedOnce();
    void runStopped();
    void runError();
    void stateChanged(NBStateManager::State s);

    void compileCountdown();

private:
    QTimer *m_t;
    NBStateManager *m_nbsm;

    bool m_stopping;
    QLabel *m_stateLbl;

    int m_compileCountdown;

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINDIALOG_H
