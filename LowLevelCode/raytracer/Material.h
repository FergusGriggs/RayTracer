#pragma once

#include "Vec3.h"
#include "ray.h"

class Material
{
public:
    Material();
    Material(const Vec3f& baseColour, float roughness = 0.7f, float metallic = 0.0f, float transmission = 0.0f, const Vec3f& emissiveColour = 0.0f);
    ~Material();

    const Vec3f& getBaseColour() const;
    const Vec3f& getEmissiveColour() const;

    float getTransmission() const;
    float getRoughness() const;
    float getMetallic() const;

    bool scatter(const Ray& in, const Vec3f& hitPosition, const Vec3f& hitNormal, Vec3f& attenuation, Ray& scattered) const;

private:
    Vec3f m_baseColour;
    Vec3f m_emissiveColour;
    
    float m_roughness;
    float m_metallic;
    float m_transmission;
};

