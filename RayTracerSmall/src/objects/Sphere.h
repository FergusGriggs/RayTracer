#pragma once

#include "Object.h"

#include "../Vec3.h"

class Sphere : public Object
{
public:
	Sphere(const Vec3f& position, const Material& material, float radius);

	// Compute a ray-sphere intersection using the geometric solution
	virtual bool intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const override;

	float getRadius() const;
	float getRadiusSquared() const;

private:
	float m_radius;
	float m_radiusSquared;
};
