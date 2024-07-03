#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include "window.h"

int main(int argc, char* argv[])
{
	hittable_list world;
    
    auto ground_material = std::make_shared<lambertian>(glm::vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(glm::vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            glm::vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random() * random();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(glm::vec3(4, 1, 0), 1.0, material3));

	camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1280;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = glm::vec3(13, 2, 3);
    cam.lookat = glm::vec3(0, 0, 0);
    cam.vup = glm::vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;


	std::vector<unsigned char> image_data(cam.image_width * cam.image_width / cam.aspect_ratio * 3);
	Window* window = new Window("App", 1280, 720, cam.image_width, cam.image_width / cam.aspect_ratio);
	cam.render(world, image_data);
	stbi_write_png("output_image.png", cam.image_width, int(cam.image_width / cam.aspect_ratio), 3, image_data.data(), cam.image_width * 3);
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