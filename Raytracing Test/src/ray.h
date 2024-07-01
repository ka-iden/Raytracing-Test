#pragma once
#include "shared.h"

class ray {
public:
    ray() {}

    ray(const glm::vec3& origin, const glm::vec3& direction) : orig(origin), dir(direction) {}

    const glm::vec3& origin() const { return orig; }
    const glm::vec3& direction() const { return dir; }

    glm::vec3 at(double t) const {
        return orig + glm::vec3(t) * dir;
    }

private:
    glm::vec3 orig;
    glm::vec3 dir;
};