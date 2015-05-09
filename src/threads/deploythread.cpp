#include "deploythread.h"
#include "global.h"
#include "log.h"

#include <QDate>
#include <QDir>

#ifdef USE_FSTREAM
#include <fstream>
#include <string>
using std::wfstream;
using std::wstring;
using std::string;
#endif

NBDeployThread::NBDeployThread() : succeed(true)
{

}

NBDeployThread::~NBDeployThread()
{

}

namespace {

    bool copyFolder(const QString &oldFolder, const QString &newFolder)
    {
        QDir newDir(newFolder);
        QDir oldDir(oldFolder);

        if (!(oldDir.exists() && !newDir.exists()))
            return false;

        if (!oldDir.mkpath(newFolder))
            return false;

        foreach (const QString &subFolderName, oldDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString oldPath = oldDir.absoluteFilePath(subFolderName);
            QString newPath = newDir.absoluteFilePath(subFolderName);
            if (!copyFolder(oldPath, newPath))
                return false;
        }

        foreach (const QString &fileName, oldDir.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
            if (!QFile::copy(oldDir.absoluteFilePath(fileName), newDir.absoluteFilePath(fileName)))
                return false;
        }

        return true;
    }

    bool removeDebugDlls(QDir &dir)
    {
        QStringList to_remove;
        foreach (const QString &filename, dir.entryList(QStringList("*d.dll"), QDir::Files)) {
            QString _filename = filename;
            _filename = _filename.replace("d.dll", ".dll");
            if (dir.exists(_filename))
                to_remove << filename;
        }

        // for VS version of Qt
        foreach (const QString &filename, dir.entryList(QStringList("*.pdb"), QDir::Files))
            to_remove << filename;

        bool result = true;
        foreach (const QString &filename, to_remove)
            result &= dir.remove(filename);

        return result;
    }

}

void NBDeployThread::run()
{
    succeed = true;

    auto writeLog = [](NBLog &logFile, const QString &logContent) -> void {
        logFile.insertLog(logContent);
    };

    // Step 0: delete the old files and folders.
    // This step isn't recorded into Log files, and we don't concern whether it succeeded or not.

    QDir globalDply = GlobalConfig::DeployPath;
    QStringList existingFileList = globalDply.entryList(QDir::Files);
    foreach (const QString &f, existingFileList)
        QFile::remove(globalDply.absoluteFilePath(f));

    QStringList existingFolderList = globalDply.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QString f, existingFolderList) {
        QDir to_delete = globalDply;
        to_delete.cd(f);
        to_delete.removeRecursively();
    }

    QDir proj(GlobalConfig::ProjectPath);
#if defined(Q_OS_WIN)
    QString dplyFilePath = proj.absoluteFilePath("bot.dply");
#elif defined(Q_OS_LINUX)
    QString dplyFilePath = proj.absoluteFilePath("bot.dplyLinux");
#endif
#ifdef USE_FSTREAM
    QFileInfo fileInfo(dplyFilePath);
    string std_path = QDir::toNativeSeparators(fileInfo.canonicalFilePath()).toStdString();
    wfstream dplyFile;
    dplyFile.open(std_path);
#else
    QFile dplyFile(dplyFilePath);
#endif
    try {
#ifndef USE_FSTREAM
        if (!dplyFile.open(QIODevice::ReadOnly | QIODevice::Text))
            throw 1;
#endif
        if (!proj.mkpath(GlobalConfig::DeployPath + "/" + QDate::currentDate().toString("yyyyMMdd")))
            throw 2;
    }
    catch (int) {
        succeed = false;
        return;
    }

    QDir dply(GlobalConfig::DeployPath + "/" + QDate::currentDate().toString("yyyyMMdd"));

    QStringList folderList, fileList, qtLibList, qtPlugList, nonQtLibList;
    QStringList *currentList = NULL;

    auto listGetter = [&](const QString &n) -> void {
        if (n == "Folders")
            currentList = &folderList;
        else if (n == "Files")
            currentList = &fileList;
        else if (n == "Qt Libraries")
            currentList = &qtLibList;
        else if (n == "Qt Plugins")
            currentList = &qtPlugList;
        else if (n == "Non Qt Libraries")
            currentList = &nonQtLibList;
        else
            currentList = NULL;
    };
#ifdef USE_FSTREAM
    wstring __line;
    while (getline(dplyFile, __line)) {
        QString l = QString::fromStdWString(__line);
#else
    while (dplyFile.canReadLine()) {
        QString l = QString::fromUtf8(dplyFile.readLine());
#endif
        l = l.trimmed();
        if (l.isEmpty())
            continue;
        if (l.startsWith('<') && l.endsWith('>')) {
            l = l.mid(1, l.length() - 2);
            listGetter(l);
        } else if (currentList != NULL)
            (*currentList) << l;
    }

    // global result
    bool ok = true;

    NBLog logFile;
    logFile.openLogFile("Deploy");

#if defined(Q_OS_WIN)
    // Step 1: copy the exe file
    writeLog(logFile, "Step 1: copy the exe file:");
    QDir buld(GlobalConfig::BuildPath);
    ok &= buld.cd("release");
    if (QFile::exists(buld.absoluteFilePath("QSanguosha.exe")))
        ok &= QFile::copy(buld.absoluteFilePath("QSanguosha.exe"), dply.absoluteFilePath("QSanguosha.exe"));

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 2: copy the folders from ProjectPath to DeployPath
    writeLog(logFile, "Step 2: copy the folders from ProjectPath to DeployPath:");
    foreach (const QString &f, folderList) {
        QString oldPath = proj.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 3: copy the files from ProjectPath to DeployPath
    writeLog(logFile, "Step 3: copy the files from ProjectPath to DeployPath:");
    foreach (const QString &f, fileList) {
        QString oldPath = proj.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= QFile::copy(oldPath, newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 4: copy the Qt and MinGW(for this instance of bot)/VS libraries to DeployPath
    writeLog(logFile, "Step 4: copy the Qt and MinGW(for this instance of bot)/VS libraries to DeployPath:");
    QDir qt(GlobalConfig::QtPath);
    ok &= qt.cd("bin");
    foreach (const QString &f, qtLibList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= QFile::copy(oldPath, newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 5: copy the Qt plugins to DeployPath
    // reminder: Remove the debug version of plugins
    writeLog(logFile, "Step 5: copy the Qt plugins to DeployPath:");
    qt = QDir(GlobalConfig::QtPath);
    ok &= qt.cd("plugins");
    foreach (const QString &f, qtPlugList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);

        QDir newDir(newPath);
        ok &= removeDebugDlls(newDir);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

#elif defined(Q_OS_LINUX)
    // Step 1: copy the exe file
    writeLog(logFile, "Step 1: copy the exe file:");
    QDir buld(GlobalConfig::BuildPath);
    if (QFile::exists(buld.absoluteFilePath("QSanguosha")))
        ok &= QFile::copy(buld.absoluteFilePath("QSanguosha"), dply.absoluteFilePath("QSanguosha"));

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 2: copy the folders from ProjectPath to DeployPath
    writeLog(logFile, "Step 2: copy the folders from ProjectPath to DeployPath:");
    foreach (const QString &f, folderList) {
        QString oldPath = proj.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 3: copy the files from ProjectPath to DeployPath
    writeLog(logFile, "Step 3: copy the files from ProjectPath to DeployPath:");
    foreach (const QString &f, fileList) {
        QString oldPath = proj.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= QFile::copy(oldPath, newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 4: copy the libraries to DeployPath/lib/linux/xxx
    writeLog(logFile, "Step 4: copy the libraries to DeployPath/lib/linux/xxx");
    QDir projlib = proj;
    ok &= projlib.cd("lib");
    ok &= projlib.cd("linux");
#if defined(LINUX_X86)
    ok &= projlib.cd("x86");
#elif defined(LINUX_X64)
    ok &= projlib.cd("x64");
#else
    ok &= projlib.cd("xxx");
#endif
    dply.mkpath("lib/linux/"
#if defined(LINUX_X86)
            "x86"
#elif defined(LINUX_X64)
            "x64"
#else
            "xxx"
#endif
    );
    QDir dplylib = dply;
    ok &= dplylib.cd("lib");
    ok &= dplylib.cd("linux");
#if defined(LINUX_X86)
    ok &= dplylib.cd("x86");
#elif defined(LINUX_X64)
    ok &= dplylib.cd("x64");
#else
    ok &= dplylib.cd("xxx");
#endif

    foreach (const QString &f, nonQtLibList) {
        QString oldPath = projlib.absoluteFilePath(f);
        QString newPath = dplylib.absoluteFilePath(f);
        ok &= QFile::copy(oldPath, newPath);
    }

    QDir qt(GlobalConfig::QtPath);
    ok &= qt.cd("lib");
    foreach (const QString &f, qtLibList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dplylib.absoluteFilePath(f);

        QFileInfo oldfileinfo(oldPath);
        oldPath = oldfileinfo.canonicalFilePath();

        ok &= QFile::copy(oldPath, newPath);
    }


    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

    // Step 5: copy the Qt plugins to DeployPath
    writeLog(logFile, "Step 5: copy the Qt plugins to DeployPath:");
    qt = QDir(GlobalConfig::QtPath);
    ok &= qt.cd("plugins");
    foreach (const QString &f, qtPlugList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);

        QDir newDir(newPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    if (!ok)
        succeed = false;

    ok = true;

#endif

    logFile.closeLogFile();

    // Step 6: copy the former log file to deploy folder
    logFile.openLogFile("Deploy-copyLog");

    writeLog(logFile, "Step 6: copy the former log file to deploy folder");
    ok &= dply.mkdir("BuildLog");
    QDir logDirtoCopy = dply;
    ok &= logDirtoCopy.cd("BuildLog");

    static QStringList toCopyLogs;
    if (toCopyLogs.isEmpty())
        toCopyLogs << "git-StdOut" << "git-StdErr" << "QMake-StdOut" << "QMake-StdErr" << "Make-StdOut" << "Make-StdErr" << "Deploy";

    QDir logDir(GlobalConfig::LogPath);
    foreach (const QString &logName, toCopyLogs) {
        QString filePath = logDir.absoluteFilePath(logName + QDate::currentDate().toString("yyyyMMdd") + ".log");
        QString toPath = logDirtoCopy.absoluteFilePath(logName + QDate::currentDate().toString("yyyyMMdd") + ".log");
        ok &= QFile::copy(filePath, toPath);
    }

    if (ok)
        writeLog(logFile, "ok\n");
    else
        writeLog(logFile, "ng\n");

    logFile.closeLogFile();

    if (!ok)
        succeed = false;
}
