#include "maindialog.h"
#include "global.h"
//#include "statemanager.h"

#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTimer>
#include <QCloseEvent>
#include <QDateTime>

namespace {
    QDateTime timeStartCompile;
}

NBMainDialog::NBMainDialog(QWidget *parent)
    : QDialog(parent), m_t(NULL), m_nbsm(NULL), m_stopping(false)
{
    m_layout =  new QVBoxLayout;
    m_pathLineLayout = new QFormLayout;
    addPathLine(tr("Project Path"), m_projectPathEdit, "ProjectPath");
    addPathLine(tr("Build Path"), m_buildPathEdit, "BuildPath");
    addPathLine(tr("Qt Path"), m_qtPathEdit, "QtPath");
    addPathLine(tr("Deploy Path"), m_deployPathEdit, "DeployPath");
    addPathLine(tr("FTP Path"), m_ftpPathEdit, "FtpPath");
    addPathLine(tr("Log Path"), m_logPathEdit, "LogPath");

    m_layout->addLayout(m_pathLineLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_stateLbl = new QLabel;
    buttonLayout->addWidget(m_stateLbl);
    m_runBtn = new QPushButton(tr("Run!"));
    buttonLayout->addWidget(m_runBtn);
    connect(m_runBtn, &QPushButton::clicked, this, &NBMainDialog::startOrStopRunning);
    m_applyBtn = new QPushButton(tr("Apply"));
    buttonLayout->addWidget(m_applyBtn);
    connect(m_applyBtn, &QPushButton::clicked, this, &NBMainDialog::saveSettings);
    m_layout->addLayout(buttonLayout);

    setLayout(m_layout);
}

NBMainDialog::~NBMainDialog()
{
    if (m_t != NULL) {
        m_t->stop();
        delete m_t;
    }
}

void NBMainDialog::closeEvent(QCloseEvent *e)
{
    if (m_nbsm != NULL) {
        e->ignore();
        return;
    }

    QDialog::closeEvent(e);
}

void NBMainDialog::addPathLine(const QString &name, QLineEdit *&edit, const QString &settingsKey)
{
    QHBoxLayout *h = new QHBoxLayout;

    edit = new QLineEdit;

    QPushButton *button = new QPushButton(tr("Browse..."));
    m_buttonEditPairs.insert(button, edit);
    connect(button, &QPushButton::released, this, &NBMainDialog::showFileDialog);

    h->addWidget(edit);
    h->addWidget(button);

    m_pathLineLayout->addRow(name, h);

    if (!settingsKey.isEmpty())
        edit->setText(NBSetting.value(settingsKey).toString());
}

void NBMainDialog::showFileDialog()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    QLineEdit *edit = m_buttonEditPairs[button];
    QString path = QFileDialog::getExistingDirectory(this, tr("Please Select A Directory"), edit->text());

    if (!path.isEmpty())
        edit->setText(path);
}

void NBMainDialog::saveSettings()
{
    GlobalConfig::ProjectPath = m_projectPathEdit->text();
    GlobalConfig::BuildPath = m_buildPathEdit->text();
    GlobalConfig::QtPath = m_qtPathEdit->text();
    GlobalConfig::DeployPath = m_deployPathEdit->text();
    GlobalConfig::FtpPath = m_ftpPathEdit->text();
    GlobalConfig::LogPath = m_logPathEdit->text();

#define SAVE_TO_CONFIG(name) NBSetting.setValue(#name, GlobalConfig::name)
    SAVE_TO_CONFIG(ProjectPath);
    SAVE_TO_CONFIG(BuildPath);
    SAVE_TO_CONFIG(QtPath);
    SAVE_TO_CONFIG(DeployPath);
    SAVE_TO_CONFIG(FtpPath);
    SAVE_TO_CONFIG(LogPath);
#undef SAVE_TO_CONFIG

}

void NBMainDialog::startOrStopRunning()
{
    if (m_nbsm != NULL) { // running
        if (m_stopping)
            return;

        if (m_t != NULL)
            m_t->stop();

        m_stopping = true;

        disconnect(m_nbsm, &NBStateManager::finished, this, 0);
        disconnect(m_nbsm, &NBStateManager::error, this, 0);

        m_nbsm->stop();
        // the rest part of this function is in NBMainDialog::runStopped()
    } else { // not running
        m_stopping = false;

        foreach (QPushButton *btn, m_buttonEditPairs.keys())
            btn->setEnabled(false);
        foreach (QLineEdit *edit, m_buttonEditPairs)
            edit->setEnabled(false);
        m_applyBtn->setEnabled(false);

        saveSettings(); // we should ask for save the changes instead of invoke this method automaticially.

        m_nbsm = new NBStateManager;
        connect(m_nbsm, &NBStateManager::finished, this, &NBMainDialog::runFinishedOnce);
        connect(m_nbsm, &NBStateManager::error, this, &NBMainDialog::runError);
        connect(m_nbsm, &NBStateManager::stopped, this, &NBMainDialog::runStopped);
        connect(m_nbsm, &NBStateManager::state_changed, this, &NBMainDialog::stateChanged);

        m_runBtn->setText(tr("Stop!"));

        startCompiling();
    }
}

void NBMainDialog::runStopped()
{
    if (!m_stopping) {
        // what the fuck?? why we don't invoke the stop method and the state manager stopped? Crash!!
        GlobalMethod::crash();
    }

    disconnect(m_nbsm, &NBStateManager::stopped, this, &NBMainDialog::runStopped);
    m_stopping = false;

    m_nbsm->deleteLater();
    m_nbsm = NULL;

    m_runBtn->setText(tr("Run"));

    foreach (QPushButton *btn, m_buttonEditPairs.keys())
        btn->setEnabled(true);
    foreach (QLineEdit *edit, m_buttonEditPairs)
        edit->setEnabled(true);
    m_applyBtn->setEnabled(true);
}

void NBMainDialog::startCompiling()
{
    if (m_nbsm == NULL) {
        // what the hell?? Don't need to explain, Crash.
        GlobalMethod::crash();
    }

    m_nbsm->start();
    timeStartCompile = QDateTime::currentDateTime();
}

void NBMainDialog::runError()
{
    // deal as normal finished
    runFinishedOnce();
}

void NBMainDialog::stateChanged(NBStateManager::State s)
{
    QString toDisplay;
    switch (s) {
    case NBStateManager::Pulling:
        toDisplay = "Pulling";
        break;
    case NBStateManager::GeneratingBotVersionCpp:
        toDisplay = "Generating bot_version.cpp";
        break;
    case NBStateManager::QMaking:
        toDisplay = "QMaking";
        break;
    case NBStateManager::Making:
        toDisplay = "Making";
        break;
    case NBStateManager::Deploying:
        toDisplay = "Deploying";
        break;
    case NBStateManager::Packaging:
        toDisplay = "Packaging";
        break;
    case NBStateManager::Uploading:
        toDisplay = "Uploading";
        break;
    case NBStateManager::Finished:
        toDisplay = "Finished";
        break;
    case NBStateManager::Error:
        toDisplay = "Error";
        break;
    default:
        toDisplay = "Unknown";
    }

    m_stateLbl->setText(toDisplay);
}

void NBMainDialog::runFinishedOnce()
{
    if (m_t == NULL)
        m_t = new QTimer;
    else
        disconnect(m_t, &QTimer::timeout, this, 0);

    QDateTime timeFinishCompile = QDateTime::currentDateTime();
    QDate d = timeStartCompile.date();
    d.addDays(1);
    QDateTime nextTimeStartCompile = QDateTime(d, timeStartCompile.time());
    //m_t->setInterval(timeFinishCompile.secsTo(nextTimeStartCompile) * 1000);
    m_t->setInterval(120000); // debug for auto re-build
    connect(m_t, &QTimer::timeout, this, &NBMainDialog::startCompiling);
}
