#include "Sphere.h"

#include <cmath>

Sphere::Sphere(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, const KeyFramedValue<float>& radiusKeyFrames) :
	Object(activeKeyFrames, positionKeyFrames, material),

	m_radiusKeyFrames(radiusKeyFrames),
	m_radiusSquared(0.0f)
{
}

void Sphere::update(float currentTime)
{
	Object::update(currentTime);

	m_radiusKeyFrames.updateCurrentValue(currentTime);
	m_radiusSquared = m_radiusKeyFrames.getCurrentValue() * m_radiusKeyFrames.getCurrentValue();
}

void Sphere::updateBoundingBox()
{
	m_boundingBox = BoundingBox::createFromSphere(getPosition(), getRadius());
}

bool Sphere::intersect(const Ray& ray, float& t0, float& t1) const
{
	Vec3f l = getPosition() - ray.m_origin;
	float tca = l.dot(ray.m_direction);
	if (tca < 0.0f)
	{
		return false;
	}

	float d2 = l.dot(l) - tca * tca;
	if (d2 > m_radiusSquared)
	{
		return false;
	}

	float thc = sqrt(m_radiusSquared - d2);

	t0 = tca - thc;
	t1 = tca + thc;

	return true;
}

float Sphere::getRadius() const
{
	return m_radiusKeyFrames.getCurrentValue();
}

float Sphere::getRadiusSquared() const
{
	return m_radiusSquared;
}
