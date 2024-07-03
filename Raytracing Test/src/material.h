#pragma once
#include "shared.h"

class hit_record;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered
    ) const {
        return false;
    }
};

class lambertian : public material {
public:
    lambertian(const glm::vec3& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered)
        const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        // Catch degenerate scatter direction
        auto s = 1e-8;
        if ((fabs(scatter_direction[0]) < s) && (fabs(scatter_direction[1]) < s) && (fabs(scatter_direction[2]) < s))
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    glm::vec3 albedo;
};

class metal : public material {
public:
    metal(const glm::vec3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered)
        const override {
        glm::vec3 reflected = glm::reflect(r_in.direction(), rec.normal);
        reflected = glm::normalize(reflected) + (glm::vec3(fuzz) * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    glm::vec3 albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered)
        const override {
        attenuation = glm::vec3(1.0, 1.0, 1.0);
        float ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;
        glm::vec3 unit_direction = glm::normalize(r_in.direction());
        
        double cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        glm::vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = glm::reflect(unit_direction, rec.normal);
        else
            direction = glm::refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);

        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};