#pragma once

#include "../vec3.h"
#include "../material.h"

#include "key_framed_value.h"
#include "../bounding_box.h"

#include "../../global_allocation.h"

struct ObjectSnapshot
{
    ObjectSnapshot(const Vec3f& position, const Material& material, bool octreeCompatible) :
        m_position(position),
        m_material(material),

        m_octreeCompatible(octreeCompatible)
    {
    }

    virtual ~ObjectSnapshot()
    {
    }

    virtual void createBoundingBox()
    {
    }

    virtual void intersect(const Ray& ray, TraceResult& traceResult) const
    {
        return;
    }

    static void* operator new(size_t size)
    {
        return ::operator new(size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
    }

    static void* operator new[](size_t size)
    {
        return ::operator new[](size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
    }

    Vec3f             m_position;
    const Material&   m_material;

    BoundingBox       m_boundingBox;

    bool m_octreeCompatible;
};

class Object
{
public:
    Object(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, bool octreeCompatible);
    virtual ~Object();

    virtual ObjectSnapshot* generateObjectSnapShotAtTime(float time) const;

    const Material& getMaterial() const;

    static void* operator new(size_t size);
    static void* operator new[](size_t size);

protected:
    KeyFramedValue<bool>  m_activeKeyFrames;
    KeyFramedValue<Vec3f> m_positionKeyFrames;

    const Material& m_material;

    bool m_octreeCompatible;
};
