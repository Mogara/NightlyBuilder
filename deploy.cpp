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
