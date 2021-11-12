#include "Object.h"

#include "../../../LowLevelCode/HeapManager.h"

Object::Object(const Vec3f& position, const Material& material) :
    m_position(position),

    m_material(material)
{
}

Object::~Object()
{
}

bool Object::intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const
{
    return false;
}

const Vec3f& Object::getPosition() const
{
    return m_position;
}

const Material& Object::getMaterial() const
{
    return m_material;
}

void* Object::operator new(size_t size)
{
    return ::operator new(size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}

void* Object::operator new[](size_t size)
{
    return ::operator new[](size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}
