#pragma once

#include "Object.h"

#include "../Vec3.h"

class Sphere : public Object
{
public:
	Sphere(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, const KeyFramedValue<float>& radiusKeyFrames);

	virtual void update(float currentTime) override;

	virtual void updateBoundingBox() override;
	// Compute a ray-sphere intersection using the geometric solution
	virtual bool intersect(const Ray& ray, float& t0, float& t1) const override;

	float              getRadius() const;
	float              getRadiusSquared() const;

private:
	KeyFramedValue<float> m_radiusKeyFrames;
	float                 m_radiusSquared;
};
