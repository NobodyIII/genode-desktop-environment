#ifndef IMAGE_RENDERER_IMAGE_LOADER_H
#define IMAGE_RENDERER_IMAGE_LOADER_H

#include <util/list.h>

#include <string.h>

#include "image_format.h"

namespace Image_renderer {

	class Image_loader
	{
	private:
		Genode::Env &_env;
		Genode::Allocator &_alloc;
		Genode::List<Image_format> _formats;

	public:
		class Invalid_file_extension { };

		Image_loader(Genode::Env &env, Genode::Allocator &alloc): _env(env), _alloc(alloc) { }

		Image *load_image(File_path path)
		{
			/* get file extension */
			const char *exten_cstr = strrchr(path.string(), '.');
			File_extension file_exten = exten_cstr;

			/* open file */
			File file(path.string(), _alloc);

			for (Image_format *format = _formats.first(); format; format = format->next()) {
				if (format->handles_extension(file_exten)) {
					/* decode image using selected format decoder */
					return format->read_image(_env, _alloc, file);
				}
			}
			Genode::error("invalid image file extension: ", file_exten.string());
			throw Invalid_file_extension();
			return 0;
		}

		void release_image(Image *image)
		{
			Genode::destroy(_alloc, image);
		}

		template<typename FORMAT> void add_format()
		{
			_formats.insert(new (_alloc) FORMAT());
		}
	};

}

#endif
