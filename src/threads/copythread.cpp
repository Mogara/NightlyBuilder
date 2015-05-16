#include "copythread.h"
#include "global.h"

#include <QDir>
#include <QFile>
#include <QDate>

NBCopyThread::NBCopyThread() : succeed(false)
{

}

NBCopyThread::~NBCopyThread()
{

}

void NBCopyThread::run()
{
    succeed = false;
    QDir ftpDir(GlobalConfig::FtpPath);

    QString fileName = QDir(GlobalConfig::ProjectPath).dirName() + "-" + QDate::currentDate().toString("yyyyMMdd") +
#if defined(Q_OS_WIN)
        ".7z"
#elif defined(Q_OS_LINUX)
#if defined(LINUX_X86)
        "-x86"
#elif defined(LINUX_X64)
        "-x64"
#else
        "-xxx"
#endif
        + ".tar.gz"
#endif
    ;

    QDir dplyDir(GlobalConfig::DeployPath);

    if (!QFile::copy(dplyDir.absoluteFilePath(fileName), ftpDir.absoluteFilePath(fileName)))
        succeed = false;

    succeed = true;
}
