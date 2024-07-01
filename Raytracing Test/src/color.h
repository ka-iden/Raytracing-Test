#pragma once
#include "interval.h"
#include "shared.h"

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return sqrt(linear_component);

    return 0;
}

void write_color(std::vector<unsigned char>& image_data, int pixel_index, const glm::vec3& pixel_color)
{
    static const interval intensity(0.000, 0.999);
    image_data[pixel_index + 0] = int(256 * intensity.clamp(linear_to_gamma(pixel_color.x)));
    image_data[pixel_index + 1] = int(256 * intensity.clamp(linear_to_gamma(pixel_color.y)));
    image_data[pixel_index + 2] = int(256 * intensity.clamp(linear_to_gamma(pixel_color.z)));
}