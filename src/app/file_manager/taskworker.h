#ifndef TASKWORKER_H
#define TASKWORKER_H

#include <atomic>

#include <QObject>
#include <QThread>
#include <QMutex>

#include "progressdialog.h"
#include "genodefs.h"
#include "fstreenode.h"

class TaskWorker;

class TimeEstimator : public QThread
{
    Q_OBJECT
public:
    TimeEstimator(TaskWorker *worker);
    void run() Q_DECL_OVERRIDE;
signals:
    void progress_update(uint time_est, quint64 size_done, quint64 size_full, quint64 speed); /* units: sec, bytes, bytes, bytes/sec (quint64 to allow extreme speeds) */
    void finished();
private:
    TaskWorker *worker;
    const quint32 sample_count=500;
};


class TaskWorker : public QObject
{
    Q_OBJECT
public:
    enum TASK {TASK_COPY, TASK_MOVE, TASK_DEL, TASK_DIR};
    static void copy_task(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path);
    static void move_task(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path); /* only use for copy-move */
    static void delete_task(GenodeFS *fs, QString dir, QStringList entries); /* no dialog */
    static void dir_task(GenodeFS *fs, QString dir_path); /* don't use a dialog */
    void start();
    void cancel();

private:
    friend class TimeEstimator;
    TaskWorker(); /* automatically create dialog */
    bool copy_file(QString src_path, quint64 size, QString dest_path);
    void copy_tree(QString src_path, FSTreeNode &root, QString dest_path);
    void populate(FSTreeNode &node, QString path);

    static int transfer_size;
    static int transfers;

    ProgressDialog *dlg;
    QThread *thread;
    TimeEstimator *time_est;

    GenodeFS *src_fs;
    GenodeFS *dest_fs;
    QMutex progress_mutex;
    TASK task;

    std::atomic<quint64> progress_bytes;
    std::atomic<quint64> size_bytes;
    std::atomic<quint64> progress_entries;
    std::atomic<quint64> size_entries;

private slots:
    void copy_thread(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path);
    void move_thread(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path);
    void delete_thread(GenodeFS *fs, QString dir, QStringList entries);
    void dir_thread(GenodeFS *fs, QString dir_path);
    void cancelled();
    void dlg_closed();
};

#endif // TASKWORKER_H
