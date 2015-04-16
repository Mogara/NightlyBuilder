#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

class QLineEdit;
class QVBoxLayout;
class QFormLayout;
class QCloseEvent;

class NBStateManager;

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

    void addPathLine(const QString &name, QLineEdit *&edit);

private slots:
    void showFileDialog();
    void saveSettings();

    void startOrStopRunning();
    void startCompiling();

    void runFinishedOnce();
    void runStopped();
    void runError();

private:
    QTimer *m_t;
    NBStateManager *m_nbsm;

    bool m_stopping;

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINDIALOG_H
