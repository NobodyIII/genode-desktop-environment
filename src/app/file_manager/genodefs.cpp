#ifndef TEST_LINUX

#include <base/env.h>
//#include <base/printf.h>
#include <file_system/util.h>
#include <libc/component.h>

#include "genodefs.h"

//using namespace Genode; /* conflict
using namespace File_system;

const int DIRENT_SIZE = sizeof(File_system::Directory_entry);

Libc::Env *GenodeFS::_env;
Genode::Heap *GenodeFS::_heap;

void GenodeFS::set_env(Libc::Env &env)
{
	_env = &env;
	_heap = new Genode::Heap(env.ram(), env.rm());
}

Node_handle GenodeFS::open_node(QString path)
{
	try {
		qDebug() << "GenodeFS::open_node():" << path;
		return fs->node(File_system::Path(path.toStdString().c_str()));
	} catch (...) {
		return Node_handle();
	}
}

Dir_handle GenodeFS::open_dir(QString path, bool create)
{
	try {
		qDebug() << "GenodeFS::open_dir():" << path << create;
		return fs->dir(File_system::Path(path.toStdString().c_str()), create);
	} catch (...) {
		return Dir_handle();
	}
}

File_handle GenodeFS::open_file(QString path, OPEN_MODE mode, bool create)
{
	Dir_handle dir;
	try {
		qDebug() << "GenodeFS::open_file():" << path << create;
		bool ro=mode==MODE_R;
		qDebug() << "GenodeFS::open(): " << (ro? "R" : "W");
		dir=parent_dir(path);
		qDebug() << "valid dir?:" << dir.valid();
		File_handle file=fs->file(dir, File_system::Name(last_element(path).toStdString().c_str()), (ro? File_system::READ_ONLY : File_system::WRITE_ONLY), create);
		qDebug() << "valid handle?:" << file.valid();
		fs->close(dir);
		return file;
	} catch (Permission_denied) {
		qDebug() << "error!"; //<< file.valid();
		qDebug() << "permission denied";
		fs->close(dir);
		return File_handle();
	} catch (Node_already_exists) {
		qDebug() << "node already exists";
	} catch (Lookup_failed) {
		qDebug() << "lookup failed";
	} catch (Name_too_long) {
		qDebug() << "name too long";
	} catch (No_space) {
		qDebug() << "no space";
	} catch (Invalid_handle) {
		qDebug() << "invalid handle";
	} catch (Invalid_name) {
		qDebug() << "invalid name";
	} catch (Not_empty) {
		qDebug() << "not empty";
	} catch (Out_of_metadata) {
		qDebug() << "out of metadata";
	} catch (...) {
		qDebug() << "unidentified error";
	}
	return File_handle();
}

Dir_handle GenodeFS::parent_dir(QString path)
{
	int delim=path.lastIndexOf("/");
	if (delim==0) return open_dir("/");
	return open_dir(path.left(delim));
}

QString GenodeFS::last_element(QString path)
{
	int delim=path.lastIndexOf("/");
	return path.mid(delim+1);
}

GenodeFS::GenodeFS(QString label): label(label)
{ }

void GenodeFS::init()
{
	qDebug() << "GenodeFS::init()";
	alloc=new Genode::Allocator_avl(_heap);
	fs=new File_system::Connection(*alloc, DEFAULT_TX_BUF_SIZE, label.toStdString().c_str());
	qDebug() << "GenodeFS::init() successful";
}

GenodeFS::~GenodeFS()
{
	delete alloc;
	delete fs;
}

bool GenodeFS::close(NodeHandle handle)
{
	try {
		if (!handle.valid()) return false;
		fs->close(handle);
		return true;
	} catch (...) {
		return false;
	}
}

bool GenodeFS::del(QString path)
{
	try {
		Dir_handle dir=parent_dir(path);
		if (!dir.valid()) return false;
		fs->unlink(dir, Name(last_element(path).toStdString().c_str()));
		fs->close(dir);
		return true;
	} catch (...) {
		return false;
	}
}

QVector<NodeStatus> GenodeFS::dir(QString dirpath)
{
	QVector<NodeStatus> nodes;
	try {
		Dir_handle dir=open_dir(dirpath);
		if (!dir.valid()) {
			qDebug() << "invalid dir";
			return nodes;
		}
		for(int i=0; true; ++i) {
			Directory_entry ent;
			if (File_system::read(*fs,dir,&ent,DIRENT_SIZE,i*DIRENT_SIZE)!=DIRENT_SIZE) break;
			ent.name[File_system::MAX_NAME_LEN-1]=0; /* ensure null terminator */
			QString name(ent.name);
			if (name=="") break;
			if (name=="." || name==".." || name=="/." || name=="/..") continue;
			nodes << stat(FSNode::canonicalize_path(dirpath + "/" + name)); /* ent.name usually begins with a slash, but not always. */
		}
		fs->close(dir);
		return nodes;
	} catch (...) {
		qDebug() << "GenodeFS::dir() failed";
		return QVector<NodeStatus>();
	}
}

QString GenodeFS::getlabel()
{
	return label;
}

bool GenodeFS::link(QString src, QString dest)
{
	qDebug() << "Function not implemented.";
	return false;
}

bool GenodeFS::mkdir(QString path)
{
	try {
		Dir_handle handle=open_dir(path, true);
		fs->close(handle);
		return true;
	} catch (...) {
		return false;
	}
}

bool GenodeFS::mkfile(QString path)
{
	try {
		File_handle handle=open_file(path, MODE_W, true);
		if (!handle.valid()) return false;
		fs->close(handle);
		return true;
	} catch (...) {
		return false;
	}
}

bool GenodeFS::move(QString src, QString dest)
{
	try {
		qDebug() << "GenodeFS::move():" << src << dest;
		Dir_handle src_dir=parent_dir(src);
		if (!src_dir.valid()) return false;
		Dir_handle dest_dir=parent_dir(dest);
		if (!dest_dir.valid()) {
			fs->close(src_dir);
			return false;
		}
		QString src_name=last_element(src);
		QString dest_name=last_element(dest);
		fs->move(src_dir, Name(src_name.toStdString().c_str()), dest_dir, Name(dest_name.toStdString().c_str()));
		fs->close(src_dir);
		fs->close(dest_dir);
		qDebug() << "GenodeFS::move(): success";
		return true;
	} catch (...) {
		return false;
	}
}

NodeHandle GenodeFS::open(QString path, OPEN_MODE mode, bool create)
{
	try {
		return open_file(path,mode,create);
	} catch (...) {
		return NodeHandle();
	}
}

bool GenodeFS::read(NodeHandle node, void *buffer, unsigned bytes, quint64 offset)
{
	try {
		if (!node.valid()) return false;
		return File_system::read(*fs, node, buffer, bytes, offset)==bytes;
	} catch (...) {
		qDebug() << "GenodeFS::read() failed";
		return false;
	}
}

bool GenodeFS::rmdir(QString path)
{
	try {
		Dir_handle handle=parent_dir(path);
		if (!handle.valid()) return false;
		fs->unlink(handle,File_system::Name(last_element(path).toStdString().c_str()));
		fs->close(handle);
		return true;
	} catch (...) {
		return false;
	}
}

NodeStatus GenodeFS::stat(QString path)
{
	try {
		Node_handle handle=open_node(path);
		if (!handle.valid()) return NodeStatus();
		File_system::Status status=fs->status(handle);
		fs->close(handle);
		int type=0;
		if (status.mode & Status::MODE_SYMLINK) type |= FSNode::ANY_SYMLINK;
		if (status.mode & Status::MODE_DIRECTORY) type |= FSNode::NODE_TYPE_DIR;
		if (status.mode & Status::MODE_FILE) type |= FSNode::NODE_TYPE_FILE;
		return NodeStatus(last_element(path),(FSNode::TYPE)type,status.size);
	} catch (...) {
		return NodeStatus();
	}
}

bool GenodeFS::write(NodeHandle node, void *buffer, unsigned bytes, quint64 offset)
{
	try {
		if (!node.valid()) return false;
		return File_system::write(*fs, node, buffer, bytes, offset)==bytes;
	} catch (...) {
		qDebug() << "GenodeFS::write() failed";
		return false;
	}
}

#endif
