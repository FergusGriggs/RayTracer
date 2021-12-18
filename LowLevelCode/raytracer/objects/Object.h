#pragma once

#include "../Vec3.h"
#include "../Material.h"

#include "KeyFramedValue.h"

class Object
{
public:
    Object(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material);
    virtual ~Object();

    virtual void update(float currentTime);

    virtual bool intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const;

    bool            isActive() const;
    const Vec3f&    getPosition() const;
    const Material& getMaterial() const;

    static void* operator new(size_t size);
    static void* operator new[](size_t size);

protected:
    KeyFramedValue<bool>  m_activeKeyFrames;
    KeyFramedValue<Vec3f> m_positionKeyFrames;

    const Material& m_material;
};

