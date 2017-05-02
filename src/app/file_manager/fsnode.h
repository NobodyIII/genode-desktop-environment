#ifndef FSNODE_H
#define FSNODE_H

#include <QString>
#include <QUrl>
#include <QDebug>

//#include "genodefs.h"
#include "fsregistry.h"
//class GenodeFS;
struct NodeStatus;

#ifdef TEST_LINUX
#include "test_linux.h"
#endif

class FSNode
{
public:
	enum TYPE {NODE_TYPE_DIR=1, NODE_TYPE_FILE=2, ANY_SYMLINK=4, NODE_TYPE_DIR_SYMLINK=6, NODE_TYPE_FILE_SYMLINK=5}; /* 6 == 2|4 and 5 == 1|4; also, ANY_SYMLINK is for & comparison */
	FSNode();
	FSNode(QString path);
	FSNode(QUrl url);
	QString getname() const;
	QString getpath() const;
	quint64 getsize() const;
	QString getfsname() const;
	QString getfspath() const;
	TYPE gettype() const;
	QUrl to_url() const;
	QList<FSNode> dir() const; /* use TaskWorker? */
	bool chdir(QString dir);
	bool copy(QString newname); /* use TaskWorker */
	bool copy(FSNode targetdir); /* use TaskWorker */
	bool mkdir(QString name);
	bool mkfile(QString name);
	bool move(FSNode targetdir); /* use TaskWorker for cross-fs */
	bool rename(QString newname);
	bool link(QString linkpath);
	bool is_valid() const;
	bool is_dir() const;
	bool is_file() const;
	bool is_symlink() const;
	bool is_hidden() const;
	bool rm(); /* use TaskWorker */
	bool refresh();

	static QString canonicalize_path(QString path);

private:
	FSNode(GenodeFS *fs, QString fs_parentdir, NodeStatus stat);

	GenodeFS *fs;
	TYPE node_type;
	QString node_full_path;
	QString node_fs;
	QString node_fs_path;
	QString node_name;
	quint64 node_size;
	QString parentdir_path;
	bool valid=false;
	unsigned buffer_size=1024*1024;
	void setpath(QString path);
	bool _copy(GenodeFS* targetfs, QString targetfspath);
	bool _copy_recursive(GenodeFS* targetfs, QString targetfspath);
	bool _rm();
	bool _rm_recursive();
	QString to_fspath(QString path);
	//QString to_fullpath(QString path);
};

#endif // FSNODE_H
