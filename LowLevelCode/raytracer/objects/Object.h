#pragma once

#include "../Vec3.h"
#include "../Material.h"

#include "KeyFramedValue.h"
#include "../bounding_box.h"

struct ObjectSnapshot
{
    ObjectSnapshot(const Vec3f& position, const Material& material) :
        m_position(position),
        m_material(material)
    {
    }

    virtual ~ObjectSnapshot()
    {
    }

    virtual void createBoundingBox()
    {
    }

    virtual bool intersect(const Ray& ray, float& t0, float& t1) const
    {
        return false;
    }

    Vec3f             m_position;
    const Material&   m_material;

    BoundingBox       m_boundingBox;
};

class Object
{
public:
    Object(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material);
    virtual ~Object();

    virtual ObjectSnapshot* generateObjectSnapShotAtTime(float time) const;

    const Material& getMaterial() const;

    static void* operator new(size_t size);
    static void* operator new[](size_t size);

protected:
    KeyFramedValue<bool>  m_activeKeyFrames;
    KeyFramedValue<Vec3f> m_positionKeyFrames;

    const Material& m_material;
};
