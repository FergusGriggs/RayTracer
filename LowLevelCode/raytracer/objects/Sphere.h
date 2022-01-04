#pragma once

#include "Object.h"

#include "../Vec3.h"

struct SphereSnapshot : ObjectSnapshot
{
	
    SphereSnapshot(const Vec3f& position, float radius, const Material& material) :
        ObjectSnapshot(position, material),
        m_radius(radius),
		m_radiusSquared(radius * radius)

    {
    }

    virtual ~SphereSnapshot()
    {
    }

    virtual void createBoundingBox()
    {
		m_boundingBox = BoundingBox::createFromSphere(m_position, m_radius);
    }

    virtual bool intersect(const Ray& ray, float& t0, float& t1) const override
    {
		Vec3f l = m_position - ray.m_origin;
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

    float m_radius;
	float m_radiusSquared;
};

class Sphere : public Object
{
public:
	Sphere(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, const KeyFramedValue<float>& radiusKeyFrames);

	virtual ObjectSnapshot* generateObjectSnapShotAtTime(float time) const override;

private:
	KeyFramedValue<float> m_radiusKeyFrames;
	float                 m_radiusSquared;
};
