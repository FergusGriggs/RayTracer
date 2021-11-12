#pragma once

#include "Vec3.h"

class Material
{
public:
    Material();
    Material(const Vec3f& baseColour, float transparency = 0.0f, float reflectivity = 0.0f, const Vec3f& emissiveColour = 0.0f);
    ~Material();

    const Vec3f& getBaseColour() const;
    const Vec3f& getEmissiveColour() const;

    float getTransparency() const;
    float getReflectivity() const;

private:
    Vec3f m_baseColour;
    Vec3f m_emissiveColour;

    float m_transparency;
    float m_reflectivity;
};

