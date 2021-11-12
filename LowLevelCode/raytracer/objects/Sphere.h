#pragma once

#include "Object.h"

#include "../Vec3.h"

class Sphere : public Object
{
public:
	Sphere(const KeyFramedValue<Vec3f>& positions, const Material& material, const KeyFramedValue<float>& radii);

	virtual void update(float currentTime) override;

	// Compute a ray-sphere intersection using the geometric solution
	virtual bool intersect(const Vec3f& rayOrigin, const Vec3f& rayDir, float& t0, float& t1) const override;

	float getRadius() const;
	float getRadiusSquared() const;

private:
	KeyFramedValue<float> m_radii;
	float                 m_radiusSquared;
};
