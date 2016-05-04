#include <QThread>
#include <QMutex>
#include "fsregistry.h"
#include "genodefs.h"

QList<GenodeFS*> filesystems;
QList<QThread*> threads;
QMutex lock;

GenodeFS *FSRegistry::get_fs(QString fsname)
{
    QMutexLocker locker(&lock);
    try  {
        foreach (GenodeFS *fs, filesystems) {
            if (fs->getlabel()==fsname) return fs;
        }
        GenodeFS *fs=new GenodeFS(fsname);
        QThread *th=new QThread();
        fs->moveToThread(th);
        th->start();
        filesystems << fs;
        threads << th;
        fs->init();
        //QMetaObject::invokeMethod(fs,"init");
        return fs;
	} catch (...) {
        return 0;
    }
}

bool FSRegistry::detach_fs(QString fsname)
{
    QMutexLocker locker(&lock);
    foreach (GenodeFS *fs, filesystems) {
        if (fs->getlabel()==fsname) {
            filesystems.removeAll(fs);
            delete fs;
            return true;
        }
    }
    return false;
}
