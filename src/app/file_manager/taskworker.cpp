#include <QElapsedTimer>

#include "taskworker.h"
#include "fstreenode.h"

int TaskWorker::transfer_size=1024*1024;
int TaskWorker::transfers=4;

void TaskWorker::copy_task(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path)
{
    TaskWorker *worker=new TaskWorker();
    worker->dlg=new ProgressDialog();
    worker->dlg->show();
    QMetaObject::invokeMethod(worker,"copy_thread",Qt::QueuedConnection,Q_ARG(GenodeFS*,src_fs),Q_ARG(QString,src_dir),Q_ARG(QStringList,src_entries),Q_ARG(GenodeFS*,dest_fs),Q_ARG(QString, dest_path));
}

void TaskWorker::move_task(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path)
{

}

void TaskWorker::delete_task(GenodeFS *fs, QString dir, QStringList entries)
{

}

void TaskWorker::dir_task(GenodeFS *fs, QString dir_path)
{

}

TaskWorker::TaskWorker(): progress_bytes(0), progress_entries(0), size_bytes(0), size_entries(0)
{
    thread=new QThread();
    moveToThread(thread);
    thread->start();
}

void TaskWorker::start()
{

}

void TaskWorker::cancel()
{

}

void TaskWorker::populate(FSTreeNode &node, QString path) /* only for dirs; path includes node name */
{
    //NodeStatus stat=src_fs->stat(path + "/" + entry);
    QVector<NodeStatus> children=src_fs->dir(path);
    foreach (NodeStatus stat, children) {
        FSTreeNode child_node(stat.name,stat.size,stat.type);
        node.children.append(child_node);
        size_entries++;
        if (child_node.type==FSNode::NODE_TYPE_DIR)
            populate(node.children.last(),path+"/"+child_node.name);
        else
            size_bytes+=child_node.size;
    }
}

bool TaskWorker::copy_file(QString src_path, quint64 size, QString dest_path)
{
    NodeHandle src=src_fs->open(src_path,GenodeFS::MODE_R,false);
    /* XXX add check to prevent overwriting */
    NodeHandle dest=dest_fs->open(dest_path,GenodeFS::MODE_W,true);

    if (!(src.valid() && dest.valid())) {
        //qDebug() << "_copy(): src valid?:" << src.valid() << "dest valid?:" << dest.valid();
        src_fs->close(src);
        dest_fs->close(dest);
        return false;
    }

    int buffer_size=transfers*transfer_size;
    int buffer_offset=0;
    void* buffer=malloc(buffer_size);
    quint64 bytes_remaining=size;
    quint64 file_offset=0;
    int readlen;
    if (size==0) {
        src_fs->close(src);
        dest_fs->close(dest);
        return true;
    }
    while (bytes_remaining) {
        readlen = bytes_remaining>transfer_size? transfer_size:bytes_remaining;
        if (!src_fs->read(src,buffer+buffer_offset,readlen,file_offset)) {
            src_fs->close(src);
            dest_fs->close(dest);
            free(buffer);
            return false;
        }
        dest_fs->write_async(dest,buffer+buffer_offset,readlen,file_offset);
        bytes_remaining-=readlen;
        file_offset+=readlen;
        progress_mutex.lock();
        progress_bytes+=readlen;
        progress_mutex.unlock();

        buffer_offset+=transfer_size;
        if (buffer_offset>=buffer_size) {
            buffer_offset=0;
            dest_fs->sync_ops();
        }
    }
    src_fs->close(src);
    dest_fs->close(dest); /* removes need to call sync_ops() */
    free(buffer);
    return true;
}

void TaskWorker::copy_tree(QString src_path, FSTreeNode &root, QString dest_path)
{
    foreach (FSTreeNode node, root.children) {
        QString child_dest=dest_path+"/"+node.name;
        QString child_src=src_path+"/"+node.name;
        if (node.type==FSNode::NODE_TYPE_DIR) {
            dest_fs->mkdir(child_dest);
            copy_tree(child_src,node,child_dest);
        } else {
            copy_file(child_src,node.size,child_dest);
        }
        progress_mutex.lock();
        ++progress_entries;
        progress_mutex.unlock();
    }
}

void TaskWorker::copy_thread(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path)
{
    this->src_fs=src_fs;
    this->dest_fs=dest_fs;
    time_est=new TimeEstimator(this);
    connect(time_est,SIGNAL(progress_update(uint,quint64,quint64,quint64)),dlg,SLOT(update_progress(uint,quint64,quint64,quint64)));
    connect(time_est,SIGNAL(finished()),dlg,SLOT(finished()));
    connect(dlg,SIGNAL(rejected()),this,SLOT(cancelled()),Qt::DirectConnection); /* cancelled will execute on calling thread */
    connect(dlg,SIGNAL(destroyed(QObject*)),this,SLOT(dlg_closed()));
    /* begin tree mapping; TODO: reduce code duplication */
    FSTreeNode root(src_dir,0,FSNode::NODE_TYPE_DIR);
    foreach (QString entry, src_entries) {
        NodeStatus stat=src_fs->stat(src_dir+"/"+entry);
        FSTreeNode entry_node(stat.name,stat.size,stat.type);
        root.children.append(entry_node);
        size_entries++;
        if (entry_node.type==FSNode::NODE_TYPE_DIR)
            populate(root.children.last(),src_dir+"/"+entry_node.name);
        else {
            size_bytes+=entry_node.size;
        }
    }
    time_est->start();
    copy_tree(src_dir,root,dest_path);
    src_fs->sync_ops();
    dest_fs->sync_ops();
}

void TaskWorker::move_thread(GenodeFS *src_fs, QString src_dir, QStringList src_entries, GenodeFS *dest_fs, QString dest_path)
{

}

void TaskWorker::delete_thread(GenodeFS *fs, QString dir, QStringList entries)
{

}

void TaskWorker::dir_thread(GenodeFS *fs, QString dir_path)
{

}

void TaskWorker::cancelled()
{

}

void TaskWorker::dlg_closed()
{
    deleteLater();
}

TimeEstimator::TimeEstimator(TaskWorker *worker) {
    this->worker=worker;
}

void TimeEstimator::run() {
    sleep(1); /* first second is always off */
    uint ents_new;
    uint ents_old;
    quint64 bytes_new;
    quint64 bytes_old;
    uint buff_ptr=0;
    uint ents[sample_count];
    double kbytes[sample_count];
    double secs[sample_count];
    QElapsedTimer timer;
    worker->progress_mutex.lock();
    uint ents_total=worker->size_entries;
    quint64 bytes_total=worker->size_bytes;
    ents_old=worker->progress_entries;
    bytes_old=worker->progress_bytes;
    worker->progress_mutex.unlock();
    timer.start();
    for (;;) {
        uint i; /* used multiple times */
        msleep(100);
        worker->progress_mutex.lock();
        ents_new=worker->progress_entries;
        bytes_new=worker->progress_bytes;
        worker->progress_mutex.unlock();
        if (ents_new==ents_total) break;
        secs[buff_ptr]=(timer.restart())/1000.0l;
        ents[buff_ptr]=ents_new-ents_old;
        kbytes[buff_ptr]=(bytes_new-bytes_old)/1000.0l;
        //qDebug() << buff_ptr;
        if (buff_ptr>50) { /* 5 sec */
            double sum_ents_sqr=0;
            double sum_kbytes_sqr=0;
            double sum_ents_x_kbytes=0;
            double ents_denom=0;
            double kbytes_denom=0;
            for (i=buff_ptr; i; --i) {
                sum_ents_sqr+=ents[i]*ents[i];
                sum_kbytes_sqr+=kbytes[i]*kbytes[i];
                sum_ents_x_kbytes+=ents[i]*kbytes[i];
            }
            for (i=buff_ptr; i; --i) {
                ents_denom+=secs[i]*(ents[i]*sum_kbytes_sqr-kbytes[i]*sum_ents_x_kbytes);
                kbytes_denom+=secs[i]*(kbytes[i]*sum_ents_sqr-ents[i]*sum_ents_x_kbytes);
            }
            double det=sum_ents_sqr*sum_kbytes_sqr-sum_ents_x_kbytes*sum_ents_x_kbytes;
            if (det && ents_denom && kbytes_denom) { /* ensure non-zero */
                double ents_per_sec=det/(double)ents_denom;
                double kbytes_per_sec=det/kbytes_denom;
                double time_remaining=((ents_total-ents_new)/ents_per_sec+((bytes_total-bytes_new)/1000.0l)/kbytes_per_sec);
                double time_total=(ents_total/ents_per_sec+(bytes_total/1000.0l)/kbytes_per_sec);
                if (time_total) emit progress_update(time_remaining,(100*time_remaining)/time_total,bytes_total,0);
                //qDebug() << timer.elapsed();
            }
        }
        if (buff_ptr>=sample_count-1) {
            /* squash old values */
            buff_ptr=sample_count/2-1;
            for (i=0; i<=buff_ptr; ++i) {
                /*ents[i]=(ents[i*2]+ents[i*2+1])*0.9;
                kbytes[i]=(kbytes[i*2]+kbytes[i*2+1])*0.9;
                secs[i]=(secs[i*2]+secs[i*2+1])*0.9;*/
                ents[i]=sqrt(ents[i*2]*ents[i*2]+ents[i*2+1]*ents[i*2+1]);
                kbytes[i]=sqrt(kbytes[i*2]*kbytes[i*2]+kbytes[i*2+1]*kbytes[i*2+1]);
                secs[i]=sqrt(secs[i*2]*secs[i*2]+secs[i*2+1]*secs[i*2+1]);
            }
        }
        ++buff_ptr;
        ents_old=ents_new;
        bytes_old=bytes_new;
    }
    emit finished();
}
