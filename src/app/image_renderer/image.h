#ifndef IMAGE_RENDERER_IMAGE_H
#define IMAGE_RENDERER_IMAGE_H

#include <os/texture_rgb565.h>
#include <os/texture_rgb888.h>
#include <os/pixel_alpha8.h>
#include <os/surface.h>
#include <os/texture.h>

#include <gems/chunky_texture.h>

namespace Image_renderer {
	typedef Genode::Pixel_rgb888 Pixel_rgb888;
	typedef Genode::Pixel_rgb565 Pixel_rgb565;
	typedef Genode::Pixel_alpha8 Pixel_alpha8;

	typedef Genode::Surface<Pixel_rgb565> Pixel_surface_rgb565;
	typedef Genode::Surface<Pixel_rgb888> Pixel_surface;
	typedef Genode::Surface<Pixel_alpha8> Alpha_surface;

	typedef Genode::Surface_base::Area  Area;
	typedef Genode::Surface_base::Point Point;
	typedef Genode::Surface_base::Rect  Rect;

	typedef Genode::Texture<Pixel_rgb888> Image_data;
	typedef Chunky_texture<Pixel_rgb888> Image_data_buffer;

	/* wrapper for cleanly releasing image data */
	struct Image
	{
		virtual Image_data &data() = 0;
		virtual ~Image() { }
	};
}

#endif
