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

class Fs_root : public Root_component<Session_component>
{
private:
	Env                     &_env;
	Heap                    &_heap { _env.ram(), _env.rm() };
	Allocator_avl           &_avl_alloc { &_heap };
	File_system::Connection &_fs;
	
public:
	Fs_root(Env &env): Root_component<Session_component>(&env.ep().rpc_ep(), &_heap)
	{
		
	}
}
 
void Component::construct(Genode::Env &env)
{
	static Fs_root inst(env);
}