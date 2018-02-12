/*
 *  \brief Server that provides access to specific files from another filesystem.
 *  \author Ben Larson
 *  \date 2017-06-20
 */

/* Genode includes */
#include <base/heap.h>
#include <file_system_session/connection.h>
#include <file_system/util.h>
#include <os/path.h>
#include <os/session_policy.h>
#include <base/heap.h>
#include <base/attached_rom_dataspace.h>
#include <root/component.h>
#include <base/component.h>
#include <base/log.h>

/* Local includes */
#include "session.h"

using namespace Genode;
using namespace File_system;

namespace FS_filter {
	class Session_component;
	class Fs_root;
}

class FS_filter::Session_component : public File_system::Session_rpc_object
{
private:
	Path                      _root_dir;
	File_system::Connection  &_fs;

public:
	Session_component(Genode::Env &env, size_t tx_buf_size, File_system::Connection fs, Path root_dir) :
		Session_rpc_object(env.ram().alloc(tx_buf_size), env.rm(), env.ep().rpc_ep()),
		_root_dir(root_dir),
		_fs(fs)
	{

	}

	Tx::Source *tx()
	{
		return _tx.source();
	}

	File_handle file(Dir_handle dir, Name const &name, Mode mode, bool create)
	{
		//TODO: fill stub
	}

	Symlink_handle symlink(Dir_handle dir, Name const &name, bool create)
	{
		//TODO: fill stub
	}

	Dir_handle dir(Path const &path, bool create)
	{
		//TODO: fill stub
	}

	Node_handle node(Path const &path)
	{
		//TODO: fill stub
	}

	void close(Node_handle node)
	{
		//TODO: fill stub
	}

	Status status(Node_handle node)
	{
		//TODO: fill stub
	}

	void control(Node_handle node, Control ctrl)
	{
		//TODO: fill stub
	}

	void unlink(Dir_handle dir, Name const &name)
	{
		//TODO: fill stub
	}

	void truncate(File_handle file, file_size_t size)
	{
		//TODO: fill stub
	}

	void move(Dir_handle source_dir, Name const &from,
		Dir_handle dest_dir, Name const &to)
	{
		//TODO: fill stub
	}
}

class FS_filter::FS_root : public Root_component<Session_component>
{
private:
	Env                     &_env;
	Heap                    &_heap { _env.ram(), _env.rm() };
	Allocator_avl           &_avl_alloc { &_heap };
	File_system::Connection &_fs;
	
public:
	FS_root(Env &env): Root_component<Session_component>(&env.ep().rpc_ep(), &_heap)
	{
		
	}
}

struct Main
{
	Env &env;
	Attached_rom_dataspace config_rom { env, "config" };

	//TODO: fill empty struct
	//TODO: use Static_root instead of using FS_root
}

void Component::construct(Genode::Env &env)
{
	static Main server(env);
}
