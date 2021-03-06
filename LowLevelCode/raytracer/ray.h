#pragma once

#include "vec3.h"

struct Ray
{
    Ray() :
        m_origin(0.0f),
        m_direction(0.0f)
    {
    }

    Ray(const Vec3f& origin, const Vec3f& direction) :
        m_origin(origin),
        m_direction(Vec3f::normalise(direction))
    {
    }

    Vec3f m_origin;
    Vec3f m_direction;
};
