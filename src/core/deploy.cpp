#include "deploy.h"
#include "globalconfig.h"

#include <QDir>

NBDeployThread::NBDeployThread() : QThread()
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
    QDir proj(GlobalConfig::ProjectPath);
    QFile dplyFile(proj.absoluteFilePath("bot.dply"));
    try {
        if (!dplyFile.open(QIODevice::ReadOnly))
            throw 1;

        if (!proj.mkpath(GlobalConfig::DeployPath))
            throw 2;
    }
    catch (int) {
        // error handling
        return;
    }

    QDir dply(GlobalConfig::DeployPath);

    QStringList folderList, fileList, qtLibList, qtPlugList;
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
        else
            currentList = NULL;
    };

    while (dplyFile.canReadLine()) {
        QString l = QString::fromUtf8(dplyFile.readLine());
        l = l.trimmed();
        if (l.startsWith('<') && l.endsWith('>')) {
            l = l.mid(1, l.length() - 2);
            listGetter(l);
        } else if (currentList != NULL)
            (*currentList) << l;
    }

    // global result
    bool ok = true;

    // Step 1: copy the exe file
    QDir buld(GlobalConfig::BuildPath);
    ok &= buld.cd("release");
    if (QFile::exists(buld.absoluteFilePath("QSanguosha.exe")))
        ok &= QFile::copy(buld.absoluteFilePath("QSanguosha.exe"), dply.absoluteFilePath("QSanguosha.exe"));

    // Step 2: copy the folders from ProjectPath to DeployPath
    foreach (const QString &f, folderList) {
        QString oldPath = proj.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);
    }

    // Step 3: copy the files from ProjectPath to DeployPath
    // reminder: there is some items that contains "->" substring in the folder list, so distinguish it
    foreach (const QString &f, fileList) {
        QString oldFileName = f;
        QString newFileName = f;
        if (f.contains("->")) {
            QStringList l = f.split("->");
            oldFileName = l.first().trimmed();
            newFileName = l.last().trimmed();
        }

        QString oldPath = proj.absoluteFilePath(oldFileName);
        QString newPath = dply.absoluteFilePath(newFileName);

        ok &= QFile::copy(oldPath, newPath);
    }

    // Step 4: copy the Qt and MinGW(for this instance of bot)/VS libraries to DeployPath
    QDir qt(GlobalConfig::QtPath);
    ok &= qt.cd("bin");
    foreach (const QString &f, qtLibList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= QFile::copy(oldPath, newPath);
    }

    // Step 5: copy the Qt plugins to DeployPath
    // reminder: Remove the debug version of plugins
    qt = QDir(GlobalConfig::QtPath);
    ok &= qt.cd("plugins");
    foreach (const QString &f, qtPlugList) {
        QString oldPath = qt.absoluteFilePath(f);
        QString newPath = dply.absoluteFilePath(f);
        ok &= copyFolder(oldPath, newPath);

        QDir newDir(newPath);
        ok &= removeDebugDlls(newDir);
    }

    if (!ok)
        //error handling 2
        return;

}
