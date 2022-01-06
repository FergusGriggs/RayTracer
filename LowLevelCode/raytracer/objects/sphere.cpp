#include "sphere.h"

#include <cmath>

Sphere::Sphere(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, const KeyFramedValue<float>& radiusKeyFrames, bool octreeCompatible) :
	Object(activeKeyFrames, positionKeyFrames, material, octreeCompatible),

	m_radiusKeyFrames(radiusKeyFrames),
	m_radiusSquared(0.0f)
{
}

ObjectSnapshot* Sphere::generateObjectSnapShotAtTime(float time) const
{
	if (m_activeKeyFrames.getValueAtTime(time))
	{
		return new SphereSnapshot(m_positionKeyFrames.getValueAtTime(time), m_radiusKeyFrames.getValueAtTime(time), m_material, m_octreeCompatible);
	}
	
	return nullptr;
}
