#include "Sphere.h"

#include <cmath>

Sphere::Sphere(const Vec3f& position, const Material& material, float radius) :
	Object(position, material),

	m_radius(radius),
	m_radiusSquared(radius* radius)
{
}

bool Sphere::intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const
{
	Vec3f l = m_position - rayOrigin;
	float tca = l.dot(rayDir);
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
	return m_radius;
}

float Sphere::getRadiusSquared() const
{
	return m_radiusSquared;
}
