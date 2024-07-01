#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include "window.h"

int main(int argc, char* argv[])
{
	hittable_list world;

	world.add(std::make_shared<sphere>(glm::vec3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(glm::vec3(0, -100.5, -1), 100));

	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	std::vector<unsigned char> image_data(cam.image_width * cam.image_width / cam.aspect_ratio * 3);
	Window* window = new Window("App", 1280, 720, cam.image_width, cam.image_width / cam.aspect_ratio);
	cam.render(world, image_data);
	stbi_write_png("output_image.png", cam.image_width, int(cam.image_width / cam.aspect_ratio),
		3, image_data.data(), cam.image_width * 3);
	while (window->Running())
	{
		if (window->HandleEvents())
		{
			cam.render(world, image_data);
		}
		window->Update(image_data, cam.image_width * 3);
		window->Render();
	}
	return 0;
}