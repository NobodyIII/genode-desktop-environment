#include <base/heap.h>
#include <base/attached_rom_dataspace.h>
#include <libc/component.h>
#include <framebuffer_session/connection.h>
#include <util/string.h>

#include "image_loader.h"
#include "framebuffer_painter.h"
#include "image_format_png.h"
#include "image_format_flif.h"

using namespace Genode;

namespace Image_renderer { struct Main; }

struct Image_renderer::Main
{
	Genode::Env &env;

	Genode::Heap heap { env.ram(), env.rm() };

	Genode::Attached_rom_dataspace config { env, "config" };

	Main(Genode::Env &env) : env(env)
	{
		/* initialize */
		Image_loader loader(env, heap);
		loader.add_format<Image_format_png>();
		loader.add_format<Image_format_flif>();

		/* load image */
		File_path image_path;
		config.xml().attribute("image").value(&image_path);

		Image *image = loader.load_image(image_path);
		Image_data &img_data = image->data();

		//TODO: switch to RGB888 when available
		//TODO: add optional width & height arguments?
		/* open framebuffer */
		Framebuffer::Mode mode(img_data.size().w(), img_data.size().h(), Framebuffer::Mode::RGB565);
		Framebuffer::Connection framebuf(env, mode);

		/* paint image to framebuffer */
		Framebuffer_painter::paint(env, heap, framebuf, img_data);
	}
};

void Libc::Component::construct(Libc::Env &env)
{
	with_libc([&env] () { static Image_renderer::Main inst(env); });
}
