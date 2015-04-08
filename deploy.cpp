#include "deploy.h"

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
        if (!(QDir::exists(oldFolder) && !QDir::exists(newFolder))) // NG!!use another method to judge the dir exists
            return false;

        QDir newDir;
        newDir.mkpath(newFolder);
        newDir.cd(newFolder);

        QDir oldDir(oldFolder);

        foreach (const QString &subFolderName, oldDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString oldPath = oldDir.absoluteFilePath(subFolderName);
            QString newPath = newDir.absoluteFilePath(subFolderName);
            copyFolder(oldPath, newPath);
        }

        foreach (const QString &fileName, oldDir.entryList(QDir::Files | QDir::NoDotAndDotDot))
            QFile::copy(oldDir.absoluteFilePath(fileName), newDir.absoluteFilePath(fileName));

    }

    bool removeDebugDlls(QDir &dir)
    {
        QStringList to_remove;
        foreach (const QString &filename, dir.entryList("*d.dll", QDir::files)) {
            QString _filename = filename;
            _filename = _filename.replace("d.dll", ".dll");
            if (dir.exists(_filename))
                to_remove << filename;
        }

        bool result = true;
        foreach (const QString &filename, to_remove)
            result &= dir.remove(filename);

        return result;
    }

}

void NBDeployThread::run()
{

}
