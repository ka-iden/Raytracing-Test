#pragma once
#include "hittable.h"

class sphere : public hittable {
public:
    sphere(const glm::vec3& center, double radius, std::shared_ptr<material> mat)
        : center(center), radius(fmax(0, radius)), mat(mat) {}


    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        glm::vec3 oc = center - r.origin();
        auto a = glm::length2(r.direction());
        auto h = glm::dot(r.direction(), oc);
        auto c = glm::length2(oc) - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / glm::vec3(radius);
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    glm::vec3 center;
    double radius;
    std::shared_ptr<material> mat;
};