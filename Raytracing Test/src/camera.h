#pragma once
#include "shared.h"
#include "hittable.h"
#include "material.h"

#include <algorithm>
#include <execution>
#include <ranges>

class camera {
public:
	double aspect_ratio = 1;  // Ratio of image width over height
	int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene

    double vfov = 90;  // Vertical view angle (field of view)
    glm::vec3 lookfrom = glm::vec3(0, 0, 0);   // Point camera is looking from
    glm::vec3 lookat = glm::vec3(0, 0, -1);  // Point camera is looking at
    glm::vec3 vup = glm::vec3(0, 1, 0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world, std::vector<unsigned char> &image_data) {
        initialize();

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\r" << (image_height - j) << " Scanlines remaining.   " << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                glm::vec3 pixel_color(0, 0, 0);
                auto sample_indices = std::vector<int>(samples_per_pixel);
                std::iota(sample_indices.begin(), sample_indices.end(), 0);
                std::for_each(std::execution::par, sample_indices.begin(), sample_indices.end(), [&](int) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                    });
                int pixel_index = (j * image_width + i) * 3;
                write_color(image_data, pixel_index, glm::vec3(pixel_samples_scale) * pixel_color);
            }
        }
        std::clog << "\rDone.                    \n";
    }

private:
	int    image_height;   // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    glm::vec3 center;         // Camera center
    glm::vec3 pixel00_loc;    // Location of pixel 0, 0
    glm::vec3   pixel_delta_u;  // Offset to pixel to the right
    glm::vec3   pixel_delta_v;  // Offset to pixel below
    glm::vec3   u, v, w;              // Camera frame basis vectors
    glm::vec3   defocus_disk_u;       // Defocus disk horizontal radius
    glm::vec3   defocus_disk_v;       // Defocus disk vertical radius

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = glm::radians(vfov);
        auto h = glm::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double)image_width / image_height;

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = glm::normalize(lookfrom - lookat);
        u = glm::normalize(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        glm::vec3 viewport_u = glm::vec3(viewport_width) * u;    // Vector across viewport horizontal edge
        glm::vec3 viewport_v = glm::vec3(viewport_height) * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / glm::vec3(image_width);
        pixel_delta_v = viewport_v / glm::vec3(image_height);

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (glm::vec3(focus_dist) * w) - viewport_u / glm::vec3(2) - viewport_v / glm::vec3(2);
        pixel00_loc = viewport_upper_left + glm::vec3(0.5) * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * glm::vec3(defocus_radius);
        defocus_disk_v = v * glm::vec3(defocus_radius);
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x) * pixel_delta_u)
            + ((j + offset.y) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    glm::vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return glm::vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    glm::vec3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

	glm::vec3 ray_color(const ray& r, int depth, const hittable& world) {
        if (depth <= 0)
            return glm::vec3(0, 0, 0);

		hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            glm::vec3 attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return glm::vec3(0, 0, 0);
        }

		glm::vec3 unit_direction = glm::normalize(r.direction());
		auto a = 0.5 * (unit_direction.y + 1.0);
		return glm::vec3(1.0 - a) * glm::vec3(1.0, 1.0, 1.0) + glm::vec3(a) * glm::vec3(0.5, 0.7, 1.0);
	}
};