#include "Object.h"

#include "../../../LowLevelCode/HeapManager.h"
#include "../GlobalAllocation.h"

Object::Object(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material) :
    m_activeKeyFrames(activeKeyFrames),
    m_positionKeyFrames(positionKeyFrames),

    m_material(material)
{
}

Object::~Object()
{
}

void Object::update(float currentTime)
{
    m_activeKeyFrames.updateCurrentValue(currentTime);
    m_positionKeyFrames.updateCurrentValue(currentTime);
}

bool Object::intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const
{
    return false;
}

bool Object::isActive() const
{
    return m_activeKeyFrames.getCurrentValue();
}

const Vec3f& Object::getPosition() const
{
    return m_positionKeyFrames.getCurrentValue();
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
