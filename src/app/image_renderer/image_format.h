#ifndef IMAGE_RENDERER_IMAGE_FORMAT_H
#define IMAGE_RENDERER_IMAGE_FORMAT_H

#include <base/allocator.h>
#include <util/string.h>
#include <gems/file.h>

#include "image.h"

namespace Image_renderer {
	typedef Genode::String<1024> File_path;
	typedef Genode::String<16> File_extension;

	class Image_format : public Genode::List<Image_format>::Element
	{
	public:
		virtual bool handles_extension(File_extension exten) = 0;
		virtual Image *read_image(Genode::Env &env, Genode::Allocator &alloc, File &file) = 0;
	};

}

#endif
