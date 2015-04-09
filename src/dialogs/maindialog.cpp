#include "maindialog.h"
#include "globalconfig.h"

#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

NBMainDialog::NBMainDialog(QWidget *parent)
    : QDialog(parent)
{
    m_layout =  new QVBoxLayout;
    addPathLine(tr("Project Path"), m_projectPathEdit);
    addPathLine(tr("Build Path"), m_buildPathEdit);
    addPathLine(tr("Qt Path"), m_qtPathEdit);
    addPathLine(tr("Deploy Path"), m_deployPathEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    QPushButton *button = new QPushButton(tr("Apply"));
    buttonLayout->addWidget(button);
    connect(button, &QPushButton::released, this, &NBMainDialog::saveSettings);
    m_layout->addLayout(buttonLayout);

    setLayout(m_layout);
}

NBMainDialog::~NBMainDialog()
{

}

void NBMainDialog::addPathLine(const QString &name, QLineEdit *&edit)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel(name));

    edit = new QLineEdit;
    layout->addWidget(edit);

    QPushButton *button = new QPushButton(tr("Browse..."));
    m_buttonEditPairs.insert(button, edit);
    layout->addWidget(button);
    connect(button, &QPushButton::released, this, &NBMainDialog::showFileDialog);

    m_layout->addLayout(layout);
}

void NBMainDialog::showFileDialog()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);
    QLineEdit *edit = m_buttonEditPairs[button];
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Please Select A Directory"),
                                                     edit->text());

    if (!path.isEmpty())
        edit->setText(path);
}

void NBMainDialog::saveSettings()
{
    using namespace GlobalConfig;

    ProjectPath = m_projectPathEdit->text();
    BuildPath = m_buildPathEdit->text();
    QtPath = m_qtPathEdit->text();
    DeployPath = m_deployPathEdit->text();
}
