#include "Object.h"

#include "../../../LowLevelCode/HeapManager.h"

Object::Object(const KeyFramedValue<Vec3f>& positions, const Material& material) :
    m_positions(positions),

    m_material(material)
{
}

Object::~Object()
{
}

void Object::update(float currentTime)
{
    m_positions.updateCurrentValue(currentTime);
}

bool Object::intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const
{
    return false;
}

const Vec3f& Object::getPosition() const
{
    return m_positions.getCurrentValue();
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
