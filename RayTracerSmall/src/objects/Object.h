#pragma once

#include "../Vec3.h"
#include "../Material.h"

class Object
{
public:
    Object(const Vec3f& position, const Material& material);
    virtual ~Object();

    virtual bool intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const;

    const Vec3f&    getPosition() const;
    const Material& getMaterial() const;

    void* operator new(size_t size);
    void* operator new[](size_t size);

protected:
    Vec3f           m_position;

    const Material& m_material;
};

