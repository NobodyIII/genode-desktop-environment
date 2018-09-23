#include <vfs/file_system.h>
#include <vfs/file_system_factory.h>
#include <vfs/env.h>

namespace Vfs_tlink {
	using namespace Genode;
	using namespace Vfs;

	enum { MAX_NAME_LEN = 128, MAX_PATH_LEN = 1024 };
	enum { STAT_MODE_FIFO = 0010000 }; /* missing from directory_service.h */
	typedef String<MAX_NAME_LEN> Name;
	typedef String<MAX_PATH_LEN> Path;

	class File_system;
}

class Vfs_tlink::File_system : public Vfs::File_system
{
private:

	Vfs::Env &_env;
	Path _target_path { };
	Name _link_name { };
	bool _writable { false };

	bool _root(const char *path)
	{
		return (strcmp(path, "") == 0) || (strcmp(path, "/") == 0);
	}

	/* compare to _link_name, removing leading slash */
	bool _link(const char *path)
	{
		return (path[0] == '/') && (_link_name == &path[1]);
	}

	bool _link_prefix(const char *path)
	{
		/* path should look like /[_link_name]/rest/of/path */
		/* _link_name.length() returns length including null terminator */
		bool match = (path[0] == '/') &&
		             (strcmp(_link_name.string(), &path[1], _link_name.length() - 1) == 0);
		char next = path[_link_name.length()];
		return match && (next == '/' || next == '\0');
	}

	Path _translate_path(const char *path)
	{
		if (_link_prefix(path))
			/* concatenate (path should begin with '/') */
			/* _link_name.length() returns length including null terminator */
			return Path(_target_path, &path[_link_name.length()]);
		else
			/* path is empty or invalid; return invalid Path */
			return Path();
	}

public:

	/**
	 * Adjust to configuration changes
	 */
	virtual void apply_config(Genode::Xml_node const &xml) override
	{
		_target_path = xml.attribute_value("target", Path());
		_link_name = xml.attribute_value("name", Name());
		_writable = xml.attribute_value("writable", false) |
					xml.attribute_value("writeable", false);
	}

	/**
	 * Return the file-system type
	 */
	char const *type() override { return "tlink"; }

	File_system(Vfs::Env &env, Genode::Xml_node config): _env(env)
	{
		apply_config(config);
	}

	/***********************
	 ** Directory service **
	 ***********************/

	Genode::Dataspace_capability dataspace(char const *path) override
	{
		Path t_path = _translate_path(path);
		if (t_path.valid())
			return _env.root_dir().dataspace(t_path.string());
		else
			return Dataspace_capability();
	}

	void release(char const *path, Dataspace_capability) override
	{
		Path t_path = _translate_path(path);
		if (t_path.valid())
			_env.root_dir().dataspace(t_path.string());
	}

	Open_result open(const char *path, unsigned mode, Vfs::Vfs_handle **out, Genode::Allocator &alloc) override
	{
		/* enforce read-only */
		if ((mode != OPEN_MODE_RDONLY) && !_writable)
			return OPEN_ERR_NO_PERM;

		Path t_path = _translate_path(path);
		if (t_path.valid())
			return _env.root_dir().open(t_path.string(), mode, out, alloc);
		else
			return Open_result::OPEN_ERR_UNACCESSIBLE;
	}

	Opendir_result opendir(char const *path, bool create, Vfs_handle **out, Allocator &alloc) override
	{
		if (_root(path)) {
			if (create)
				return OPENDIR_ERR_PERMISSION_DENIED;

			/* try to create handle */
			Vfs_handle *root_handle;
			try {
				root_handle = new (alloc) Vfs_handle(*this, *this, alloc, Vfs_handle::STATUS_RDONLY);
			}
			catch (Genode::Out_of_ram)  { return OPENDIR_ERR_OUT_OF_RAM; }
			catch (Genode::Out_of_caps) { return OPENDIR_ERR_OUT_OF_CAPS; }

			/* success */
			*out = root_handle;
			return OPENDIR_OK;
		}

		if (create && !_writable)
			return OPENDIR_ERR_PERMISSION_DENIED;

		Path t_path = _translate_path(path);

		if (t_path.valid())
			return _env.root_dir().opendir(t_path.string(), create, out, alloc);
		else
			return OPENDIR_ERR_LOOKUP_FAILED;

	}

	void close(Vfs::Vfs_handle *handle) override
	{
		/* if handle is valid and we own it, destroy it */
		if (handle && (&handle->ds() == this))
			destroy(handle->alloc(), handle);
	}

	virtual Watch_result watch(char const *path, Vfs_watch_handle **handle, Allocator &alloc)
	{
		Path t_path = _translate_path(path);
		if (t_path.valid())
			return _env.root_dir().watch(t_path.string(), handle, alloc);
		else
			return WATCH_ERR_UNACCESSIBLE;
	}

	virtual void close(Vfs::Vfs_watch_handle *handle)
	{
		/* if handle is valid and we own it, destroy it */
		if (handle && (&handle->fs() == this))
			destroy(handle->alloc(), handle);
	};

	/* stat() passthrough with exceptions for special cases */
	Stat_result stat(const char *path, Vfs::Directory_service::Stat &out) override
	{
		if (_root(path)) {
			out = Stat();
			out.device = (Genode::addr_t)this;
			out.mode = STAT_MODE_DIRECTORY;
			return STAT_OK;
		}

		Path t_path = _translate_path(path);

		if (t_path.valid())
			return _env.root_dir().stat(t_path.string(), out);
		else
			return STAT_ERR_NO_ENTRY;
	}

	Unlink_result unlink(const char *path) override
	{
		/* enforce readonly access */
		if (!_writable)
			return UNLINK_ERR_NO_PERM;

		Path t_path = _translate_path(path);

		if (t_path.valid())
			return _env.root_dir().unlink(t_path.string());
		else if (_root(path))
			return UNLINK_ERR_NO_PERM;
		else
			return UNLINK_ERR_NO_ENTRY;
	}

	Rename_result rename(const char *from, const char *to) override
	{
		/* enforce readonly access */
		if (!_writable)
			return RENAME_ERR_NO_PERM;

		/* forbid link renaming */
		if (_link(from) || _link(to))
			return RENAME_ERR_NO_PERM;

		Path t_path_from = _translate_path(from);
		Path t_path_to = _translate_path(to);

		if (!t_path_from.valid() || !t_path_to.valid())
			return RENAME_ERR_NO_ENTRY;

		return _env.root_dir().rename(t_path_from.string(), t_path_to.string());
	}

	file_size num_dirent(const char *path) override
	{
		if (_root(path))
			return 1;

		Path t_path = _translate_path(path);

		if (t_path.valid())
			return _env.root_dir().num_dirent(t_path.string());
		else
			return 0;
	}

	bool directory(char const *path) override
	{
		if (_root(path))
			return true;

		Path t_path = _translate_path(path);
		if (t_path.valid())
			return _env.root_dir().directory(t_path.string());
		else
			return false;
	}

	const char* leaf_path(const char *path) override
	{
		if (_link(path))
			return path;

		Path t_path = _translate_path(path);
		if (t_path.valid())
			return _env.root_dir().leaf_path(t_path.string());
		else
			return 0;
	}

	/**********************
	 ** File I/O service **
	 **********************/

	/* these functions should only be called on handles for root ('/') */

	Write_result write(Vfs_handle*, const char *, file_size, file_size&) override
	{
		return WRITE_ERR_INVALID;
	}

	Read_result complete_read(Vfs_handle *handle, char *dst, file_size count, file_size &out_count) override
	{
		out_count = 0;

		/* only read whole Dirent at a time */
		if (count < sizeof(Dirent))
			return READ_ERR_INVALID;

		/* create Dirent in dst buffer */
		Dirent *dirent = (Dirent*)dst;
		file_offset index = handle->seek() / sizeof(Dirent);

		if (index == 0) { /* first entry (link) */
			strncpy(dirent->name, _link_name.string(), sizeof(dirent->name));
			Stat stat;
			if (_env.root_dir().stat(_target_path.string(), stat) == STAT_OK) {
				dirent->fileno = 1;
				/* convert stat modes to dirent types */
				if (stat.mode & STAT_MODE_FIFO)
					dirent->type = DIRENT_TYPE_FIFO;
				else if (stat.mode & STAT_MODE_BLOCKDEV)
					dirent->type = DIRENT_TYPE_BLOCKDEV;
				else if (stat.mode & STAT_MODE_CHARDEV)
					dirent->type = DIRENT_TYPE_CHARDEV;
				else if (stat.mode & STAT_MODE_SYMLINK)
					dirent->type = DIRENT_TYPE_SYMLINK;
				else if (stat.mode & STAT_MODE_DIRECTORY)
					dirent->type = DIRENT_TYPE_DIRECTORY;
				else
					dirent->type = DIRENT_TYPE_FILE;
			} else { /* error with stat(); assume file */
				dirent->type = DIRENT_TYPE_FILE;
			}
		} else { /* no entry (end) */
			dirent->type = DIRENT_TYPE_END;
		}

		out_count = sizeof(Dirent);
		return READ_OK;
	}

	bool read_ready(Vfs_handle*) override
	{
		return true;
	}

	Ftruncate_result ftruncate(Vfs_handle*, file_size) override
	{
		return FTRUNCATE_ERR_NO_PERM;
	}

	Sync_result complete_sync(Vfs_handle*) override
	{
		return SYNC_OK;
	}
};

extern "C" Vfs::File_system_factory *vfs_file_system_factory(void)
{
	struct Factory : Vfs::File_system_factory
	{
		Vfs::File_system *create(Vfs::Env &env, Genode::Xml_node config) override
		{
			return new (env.alloc())
				Vfs_tlink::File_system(env, config);
		}
	};

	static Factory f;
	return &f;
}

