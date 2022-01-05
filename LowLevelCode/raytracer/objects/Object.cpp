#include "Object.h"

#include "../../heap_manager.h"

Object::Object(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, bool octreeCompatible) :
    m_activeKeyFrames(activeKeyFrames),
    m_positionKeyFrames(positionKeyFrames),

    m_material(material),

    m_octreeCompatible(octreeCompatible)
{
}

Object::~Object()
{
}

const Material& Object::getMaterial() const
{
    return m_material;
}

ObjectSnapshot* Object::generateObjectSnapShotAtTime(float time) const
{
    if (m_activeKeyFrames.getValueAtTime(time))
    {
        return new ObjectSnapshot(m_positionKeyFrames.getValueAtTime(time), m_material, m_octreeCompatible);
    }

    return nullptr;
}

void* Object::operator new(size_t size)
{
    return ::operator new(size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}

void* Object::operator new[](size_t size)
{
    return ::operator new[](size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}
