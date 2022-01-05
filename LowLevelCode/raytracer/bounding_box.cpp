#include "bounding_box.h"

BoundingBox::BoundingBox() :
    m_minPoint(Vec3f(0.0f)),
    m_extents(Vec3f(0.0f))
{
}

BoundingBox::BoundingBox(const Vec3f& minPoint, const Vec3f& extents) :
    m_minPoint(minPoint),
    m_extents(extents)
{
}

BoundingBox::~BoundingBox()
{
}

bool BoundingBox::intersects(const BoundingBox& other) const
{
    Vec3f selfMax = m_minPoint + m_extents;
    Vec3f otherMax = other.m_minPoint + other.m_extents;

    bool disjointed = Vec3<bool>::isAnyTrue(Vec3<bool>::boolVecOr(m_minPoint > otherMax, other.m_minPoint > selfMax));

    return !disjointed;
}

bool BoundingBox::intersects(const Ray& ray, const Vec3f& invDir) const
{
    Vec3f maxPoint = m_minPoint + m_extents;

    float t1 = (m_minPoint.x - ray.m_origin.x) * invDir.x;
    float t2 = (maxPoint.x - ray.m_origin.x) * invDir.x;
    float t3 = (m_minPoint.y - ray.m_origin.y) * invDir.y;
    float t4 = (maxPoint.y - ray.m_origin.y) * invDir.y;
    float t5 = (m_minPoint.z - ray.m_origin.z) * invDir.z;
    float t6 = (maxPoint.z - ray.m_origin.z) * invDir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        return false;
    }

    return true;
}

bool BoundingBox::contains(const BoundingBox& other) const
{
    Vec3f selfMax = m_minPoint + m_extents;
    Vec3f otherMax = other.m_minPoint + other.m_extents;

    bool disjointed = Vec3<bool>::isAnyTrue(Vec3<bool>::boolVecOr(m_minPoint > otherMax, other.m_minPoint > selfMax));

    if (disjointed)
    {
        return false;
    }

    bool contained = Vec3<bool>::areAllTrue(Vec3<bool>::boolVecAnd(m_minPoint <= other.m_minPoint, otherMax <= selfMax));

    return contained;
}

void BoundingBox::mergeWithBoundingBox(const BoundingBox& other)
{
    Vec3f min = Vec3f::min(m_minPoint, other.m_minPoint);
    Vec3f max = Vec3f::max(m_minPoint + m_extents, other.m_minPoint + other.m_extents);

    m_minPoint = min;
    m_extents = max - min;
}

BoundingBox BoundingBox::createFromSphere(const Vec3f& position, float radius)
{
    Vec3f min = position - radius;
    Vec3f max = position + radius;
    
    return BoundingBox(min, max - min);
}
