#ifndef GENODEFS_H
#define GENODEFS_H

#ifndef TEST_LINUX

#include <base/allocator_avl.h>
#include <file_system_session/connection.h>

//using namespace Genode; /* conflicting definitions (e.g. memcpy, strlen) */
//using namespace File_system;

typedef File_system::File_handle NodeHandle; /* XXX change NodeHandle to FileHandle */

#endif

#include <QStringList>
#include "fsnode.h"
class ERR_NO_FS {};

struct NodeStatus
{
    QString name="invalid";
    FSNode::TYPE type;
    quint64 size;
    NodeStatus() { }
    NodeStatus(QString node_name, FSNode::TYPE node_type, quint64 node_size): name(node_name), type(node_type), size(node_size) { }
    bool valid() {return name=="invalid" ? false : true;}
};

class GenodeFS : public QObject
{
    Q_OBJECT
public:
    enum OPEN_MODE {MODE_R=1, MODE_W=2}; //, MODE_RW=3};/* MODE_RW = MODE_R | MODE_W */
    GenodeFS(QString label);
    ~GenodeFS();

    QString getlabel();

    inline void init() {
        QMetaObject::invokeMethod(this,"_getlabel",Qt::QueuedConnection);
    }

    inline NodeStatus stat(QString path) {
        NodeStatus ret;
        QMetaObject::invokeMethod(this,"_stat",Qt::BlockingQueuedConnection,Q_RETURN_ARG(NodeStatus,ret),Q_ARG(QString,path));
        return ret;
    }

    /*inline QStringList dir(QString dirpath) {
        QStringList ret;
        QMetaObject::invokeMethod(this,"_dir",Qt::BlockingQueuedConnection,Q_RETURN_ARG(QStringList,ret),Q_ARG(QString,dirpath));
        return ret;
    }*/

    inline QVector<NodeStatus> dir(QString dirpath) {
        QVector<NodeStatus> ret;
        QMetaObject::invokeMethod(this,"_dir",Qt::BlockingQueuedConnection,Q_RETURN_ARG(QVector<NodeStatus>,ret),Q_ARG(QString,dirpath));
        return ret;
    }

    inline NodeHandle open(QString path, GenodeFS::OPEN_MODE mode, bool create) {
        NodeHandle ret;
        QMetaObject::invokeMethod(this,"_open",Qt::BlockingQueuedConnection,Q_RETURN_ARG(NodeHandle,ret),Q_ARG(QString,path),Q_ARG(OPEN_MODE,mode),Q_ARG(bool,create));
        return ret;
    }

    inline bool read(NodeHandle node, void *buffer, int bytes, quint64 offset=0) {
        bool ret;
        QMetaObject::invokeMethod(this,"_read",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(NodeHandle,node),Q_ARG(void*,buffer),Q_ARG(int,bytes),Q_ARG(quint64,offset));
        return ret;
    }

    inline bool write(NodeHandle node, void *buffer, int bytes, quint64 offset=0) {
        bool ret;
        QMetaObject::invokeMethod(this,"_write",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(NodeHandle,node),Q_ARG(void*,buffer),Q_ARG(int,bytes),Q_ARG(quint64,offset));
        return ret;
    }

    inline void write_async(NodeHandle node, void *buffer, int bytes, quint64 offset=0) {
        QMetaObject::invokeMethod(this,"_write",Qt::QueuedConnection,Q_ARG(NodeHandle,node),Q_ARG(void*,buffer),Q_ARG(int,bytes),Q_ARG(quint64,offset));
    }

    inline bool sync_ops() {
        QMetaObject::invokeMethod(this,"_dummy",Qt::BlockingQueuedConnection);
        return true;
    }

    inline bool move(QString src, QString dest) {
        bool ret;
        QMetaObject::invokeMethod(this,"_move",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,src),Q_ARG(QString,dest));
        return ret;
    }

    inline bool link(QString src, QString dest) {
        bool ret;
        QMetaObject::invokeMethod(this,"_link",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,src),Q_ARG(QString,dest));
        return ret;
    }

    inline bool del(QString path) {
        bool ret;
        QMetaObject::invokeMethod(this,"_del",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,path));
        return ret;
    }

    inline bool close(NodeHandle handle) {
        bool ret;
        QMetaObject::invokeMethod(this,"_close",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(NodeHandle,handle));
        return ret;
    }

    inline bool mkfile(QString path) {
        bool ret;
        QMetaObject::invokeMethod(this,"_mkfile",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,path));
        return ret;
    }

    inline bool mkdir(QString path) {
        bool ret;
        QMetaObject::invokeMethod(this,"_mkdir",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,path));
        return ret;
    }

    inline bool rmdir(QString path) {
        bool ret;
        QMetaObject::invokeMethod(this,"_rmdir",Qt::BlockingQueuedConnection,Q_RETURN_ARG(bool,ret),Q_ARG(QString,path));
        return ret;
    }

private slots:
    void _init();
    void _dummy() { }
    NodeStatus _stat(QString path);
    //QStringList _dir(QString dirpath);
    QVector<NodeStatus> _dir(QString dirpath);
    NodeHandle _open(QString path, OPEN_MODE mode, bool create);
    bool _read(NodeHandle node, void *buffer, int bytes, quint64 offset=0); /* use stat to determine size */
    bool _write(NodeHandle node, void *buffer, int bytes, quint64 offset=0);
    bool _move(QString src, QString dest);
    bool _link(QString src, QString dest);
    bool _del(QString path);
    bool _close(NodeHandle handle);
    bool _mkfile(QString path);
    bool _mkdir(QString path);
    bool _rmdir(QString path);

#ifndef TEST_LINUX
    
private:
    File_system::Node_handle open_node(QString path);
    File_system::Dir_handle open_dir(QString path, bool create=false);
    File_system::File_handle open_file(QString path, OPEN_MODE mode, bool create=false);
    File_system::Dir_handle parent_dir(QString path);
    QString last_element(QString path);
    
    Genode::Allocator_avl *alloc;
    File_system::Connection *fs;
    QString label;
    
#endif
};

typedef GenodeFS::OPEN_MODE OPEN_MODE;


#endif // GENODEFS_H
