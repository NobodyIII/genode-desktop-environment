#ifdef TEST_LINUX
#include <QDir>
#include <QDebug>

#include "test_linux.h"
#include "genodefs.h"

QList<QFile> open_nodes;
QDir rootdir("/");

GenodeFS::GenodeFS(QString label)
{

}

GenodeFS::~GenodeFS()
{

}

void GenodeFS::_init() { }

QString GenodeFS::getlabel()
{
	return "main";
}

NodeStatus GenodeFS::_stat(QString path)
{
	QFileInfo info(path);
    //info.refresh();
	NodeStatus status(info.fileName(), info.isDir()? FSNode::NODE_TYPE_DIR : FSNode::NODE_TYPE_FILE, info.size());
	return status;
}

/*QStringList GenodeFS::_dir(QString dirpath)
{
	QDir dir("/");
    if (!dir.cd(dirpath)) return QStringList();
    return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
}*/

QVector<NodeStatus> GenodeFS::_dir(QString dirpath)
{
    QStringList nodes;
    QDir dir("/");
    if (dir.cd(dirpath))
        nodes = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    QVector<NodeStatus> nodestats;
    nodestats.reserve(nodes.size());
    foreach (QString node, nodes) {
        QFileInfo info(dirpath+"/"+node);
        //info.refresh();
        nodestats << NodeStatus(info.fileName(), info.isDir()? FSNode::NODE_TYPE_DIR : FSNode::NODE_TYPE_FILE, info.size());
    }
    return nodestats;
}

NodeHandle GenodeFS::_open(QString path, OPEN_MODE mode, bool create)
{
    //qDebug() << "GenodeFS::open";
	QFile* node=new QFile(path);
	NodeHandle handle;
	bool ro=mode==MODE_R;
    //qDebug() << "GenodeFS::open(): " << (ro? "R" : "W");
	if (node->open(ro? QFile::ReadOnly : QFile::WriteOnly))
		handle.file.reset(node);
	else
		delete node;
	return handle;
}

bool GenodeFS::_read(NodeHandle node, void *buffer, int bytes, quint64 offset)
{
    //qDebug() << "GenodeFS::read";
    qint64 bytes_read=node.file->read((char*)buffer,bytes);
    //qDebug() << "bytes read: " << QString::number(bytes_read);
	return bytes_read==bytes;
}

bool GenodeFS::_write(NodeHandle node, void *buffer, int bytes, quint64 offset)
{
    //qDebug() << "GenodeFS::write";
    return node.file->write((char*)buffer,bytes);
}

bool GenodeFS::_move(QString src, QString dest)
{
    //qDebug() << "GenodeFS::move(): " << src << " -> " << dest;
	return rootdir.rename(src,dest);
}

bool GenodeFS::_link(QString src, QString dest)
{
    //qDebug() << "GenodeFS::link"; /* XXX add link code */
}

bool GenodeFS::_del(QString path)
{
    //qDebug() << "GenodeFS::del(): " << path;
	return rootdir.remove(path);
}

bool GenodeFS::_close(NodeHandle handle)
{
    if (handle.file.isNull()) return true;
	handle.file->close();
	return true;
}

bool GenodeFS::_mkdir(QString path)
{
    //qDebug() << "GenodeFS::mkdir(): " << path;
    return rootdir.mkpath(path);
}

bool GenodeFS::_rmdir(QString path)
{
    //qDebug() << "GenodeFS::rmdir(): " << path;
    return rootdir.rmdir(path);
}

bool GenodeFS::_mkfile(QString path)
{
    //qDebug() << "GenodeFS::mkfile(): " << path;
    QFile f(path);
    f.open(QFile::WriteOnly);
    f.close();
}

#endif
