#include "stategeneratingbotversioncpp.h"
#include "globalconfig.h"

#include <QDir>
#include <QFile>
#include <QDate>

NBStateGeneratingBotVersionCpp::NBStateGeneratingBotVersionCpp(QObject *parent) : NBState(parent)
{

}

NBStateGeneratingBotVersionCpp::~NBStateGeneratingBotVersionCpp()
{

}


void NBStateGeneratingBotVersionCpp::run()
{
    QDir d(GlobalConfig::ProjectPath);

    if (!d.exists()) {
        emit fatal();
        return;
    }

    if (!d.cd("src")) {
        emit fatal();
        return;
    }

    QString p = d.filePath("bot_version.cpb");
    if (!QFile::exists(p)) {
        emit fatal();
        return;
    }

    QFile f(p);
    f.open(QIODevice::ReadOnly);

    QByteArray ba = f.readAll();
    f.close();

    ba.replace("%date", QDate::currentDate().toString("yyyyMMdd").toLatin1());

    p.replace(".cpb", ".cpp");

    QFile t(p);
    t.open(QIODevice::WriteOnly | QIODevice::Truncate);

    t.write(ba);
    t.close();

    emit finished();
}

void NBStateGeneratingBotVersionCpp::shutUp()
{
    // does nothing!!!
}
