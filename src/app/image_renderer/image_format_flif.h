#ifndef IMAGE_RENDERER_IMAGE_FORMAT_FLIF
#define IMAGE_RENDERER_IMAGE_FORMAT_FLIF

#include <flif_dec.h>

#include "image_format.h"

namespace Image_renderer {

	class Image_format_flif : public Image_format
	{
	public:
		struct Image_flif : Image
		{
			Genode::Env        &env;
			Genode::Allocator  &alloc;
			Image_data_buffer  *image;

			Image_flif(Genode::Env &env, Genode::Allocator &alloc, File &file):
				env(env), alloc(alloc)
			{
				/* create decoder */
				FLIF_DECODER *flif_dec = flif_create_decoder();

				/* decode FLIF image */
				flif_decoder_decode_memory(flif_dec, file.data<void>(), file.size());
				FLIF_IMAGE *flif_img = flif_decoder_get_image(flif_dec, 0);


				/* create texture */
				uint32_t width = flif_image_get_width(flif_img);
				uint32_t height = flif_image_get_height(flif_img);
				Area size(width, height);
				image = new (alloc) Image_data_buffer(env.ram(), env.rm(), size);

				/* copy image into texture */
				for (uint32_t row = 0; row < height; ++row) {
					unsigned char row_buffer[width * 4];
					flif_image_read_row_RGBA8(flif_img, row, row_buffer, width * 4);
					image->rgba(row_buffer, width, row);
				}

				/* cleanup */
				flif_destroy_decoder(flif_dec);
			}

			~Image_flif()
			{
				Genode::destroy(alloc, image);
			}

			Image_data &data() { return *image; }
		};

		bool handles_extension(File_extension exten)
		{
			return (exten == ".flif" || exten == ".FLIF");
		}

		Image *read_image(Genode::Env &env, Genode::Allocator &alloc, File &file)
		{
			return new (alloc) Image_flif(env, alloc, file);
		}

	};

}

#endif
