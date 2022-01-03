#include "bounding_box.h"

BoundingBox::BoundingBox(const Vec3f& minPoint, const Vec3f& extents)
{
}

BoundingBox::~BoundingBox()
{
}

bool BoundingBox::intersects(const BoundingBox& other)
{
    Vec3f selfMax = m_minPoint + m_extents;
    Vec3f otherMax = other.m_minPoint + other.m_extents;

    bool disjointed = Vec3<bool>::isAnyTrue((m_minPoint > otherMax) || (other.m_minPoint > selfMax));

    return !disjointed;
}

bool BoundingBox::contains(const BoundingBox& other)
{
    XMVECTOR CenterA = XMLoadFloat3(&Center);
    XMVECTOR ExtentsA = XMLoadFloat3(&Extents);

    XMVECTOR CenterB = XMLoadFloat3(&box.Center);
    XMVECTOR ExtentsB = XMLoadFloat3(&box.Extents);

    XMVECTOR MinA = XMVectorSubtract(CenterA, ExtentsA);
    XMVECTOR MaxA = XMVectorAdd(CenterA, ExtentsA);

    XMVECTOR MinB = XMVectorSubtract(CenterB, ExtentsB);
    XMVECTOR MaxB = XMVectorAdd(CenterB, ExtentsB);

    // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return false
    XMVECTOR Disjoint = XMVectorOrInt(XMVectorGreater(MinA, MaxB), XMVectorGreater(MinB, MaxA));

    if (DirectX::Internal::XMVector3AnyTrue(Disjoint))
        return DISJOINT;

    // for each i in (x, y, z) if a_min(i) <= b_min(i) and b_max(i) <= a_max(i) then A contains B
    XMVECTOR Inside = XMVectorAndInt(XMVectorLessOrEqual(MinA, MinB), XMVectorLessOrEqual(MaxB, MaxA));

    return DirectX::Internal::XMVector3AllTrue(Inside) ? CONTAINS : INTERSECTS;
}

BoundingBox BoundingBox::createFromSphere(const Vec3f& position, float radius)
{
    Vec3f min = position - radius;
    Vec3f max = position + radius;
    
    return BoundingBox(min, max - min);
}

void BoundingBox::mergeBoundingBoxIntoFirst(BoundingBox& first, const BoundingBox& other)
{
    Vec3f min = Vec3f::min(first.m_minPoint, other.m_minPoint);
    Vec3f max = Vec3f::max(first.m_minPoint + first.m_extents, other.m_minPoint + other.m_extents);

    first.m_minPoint = min;
    first.m_extents = max - min;
}
