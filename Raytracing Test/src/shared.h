#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/random.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline glm::vec3 random() {
    return glm::vec3(random_double(), random_double(), random_double());
}

inline glm::vec3 random(double min, double max) {
    return glm::vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline glm::vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1, 1);
        if (glm::length2(p) < 1)
            return p;
    }
}

inline glm::vec3 random_unit_vector() {
    return glm::normalize(random_in_unit_sphere());
}

inline glm::vec3 random_on_hemisphere(const glm::vec3& normal) {
    glm::vec3 on_unit_sphere = random_unit_vector();
    if (glm::dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Common Headers

#include "color.h"
#include "ray.h"
#include "interval.h"
