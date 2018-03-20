#ifndef IMAGE_RENDERER_FRAMEBUFFER_PAINTER_H
#define IMAGE_RENDERER_FRAMEBUFFER_PAINTER_H

#include <base/attached_dataspace.h>
#include <framebuffer_session/connection.h>
#include <os/dither_painter.h>

#include <gems/texture_utils.h>

#include "image.h"

namespace Image_renderer {

	struct Framebuffer_painter
	{
		static void paint(Genode::Env &env, Genode::Allocator &alloc, Framebuffer::Connection &framebuf, Image_data &image)
		{
			/* initialize */
			Genode::Attached_dataspace fb_ds(env.rm(), framebuf.dataspace());
			Framebuffer::Mode fb_mode = framebuf.mode();
			Area fb_area(fb_mode.width(), fb_mode.height());

			/* scale texture */
			Image_data_buffer scaled_image(env.ram(), env.rm(), fb_area);
			scale(image, scaled_image, alloc);

			/* paint RGB */
			Pixel_surface_rgb565 fb_surface(fb_ds.local_addr<Pixel_rgb565>(), fb_area);
			Dither_painter::paint(fb_surface, scaled_image, Point(0, 0));

			/* determine whether framebuffer has an alpha channel */
			bool has_alpha = (fb_ds.size() >= ((fb_mode.bytes_per_pixel() + 1) * fb_area.count()));

			/* paint alpha (if available) */
			if (has_alpha) {
				/* map framebuffer alpha channel */
				void *fb_alpha = fb_ds.local_addr<char>() + (fb_mode.bytes_per_pixel() * fb_area.count());

				/* because scaled_image is the same size as framebuf, we can just memcpy/blit the alpha channel */
				Genode::memcpy(fb_alpha, scaled_image.alpha(), fb_area.count());
			}

			/* refresh */
			framebuf.refresh(0, 0, fb_mode.width(), fb_mode.height());
		}
	};
}

#endif
