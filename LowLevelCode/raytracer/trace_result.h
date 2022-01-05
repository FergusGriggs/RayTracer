#pragma once

#include "vec3.h"

// Fwd declare
struct ObjectSnapshot;

struct TraceResult
{
    TraceResult() :
        m_hitOccured(false),
        m_hitPoint(0.0f),
        m_hitNormal(0.0f, 1.0f, 0.0f),
        m_object(nullptr),
        m_hitDistance(INFINITY),
        m_inside(false)
    {
    }

    bool                  m_hitOccured;
    Vec3f                 m_hitPoint;
    Vec3f                 m_hitNormal;
    const ObjectSnapshot* m_object;
    float                 m_hitDistance;
    bool                  m_inside;
};