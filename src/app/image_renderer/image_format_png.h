#ifndef IMAGE_RENDERER_IMAGE_FORMAT_PNG
#define IMAGE_RENDERER_IMAGE_FORMAT_PNG

#include <gems/png_image.h>

#include "image_format.h"

namespace Image_renderer {

	class Image_format_png : public Image_format
	{
	public:
		struct Image_png : Image
		{
			Png_image png;
			Image_data *image;

			Image_png(Genode::Env &env, Genode::Allocator &alloc, File &file):
				png(env.ram(), env.rm(), alloc, file.data<void>()),
				image(png.texture<Pixel_rgb888>()) { }

			~Image_png()
			{
				png.release_texture(image);
			}

			Image_data &data() { return *image; }
		};

		bool handles_extension(File_extension exten)
		{
			return (exten == ".png" || exten == ".PNG");
		}

		Image *read_image(Genode::Env &env, Genode::Allocator &alloc, File &file)
		{
			return new (alloc) Image_png(env, alloc, file);
		}

	};

}

#endif
