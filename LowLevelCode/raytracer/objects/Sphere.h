#pragma once

#include "object.h"

#include "../vec3.h"

struct SphereSnapshot : ObjectSnapshot
{
	
    SphereSnapshot(const Vec3f& position, float radius, const Material& material, bool octreeCompatible) :
        ObjectSnapshot(position, material, octreeCompatible),
        m_radius(radius),
		m_radiusSquared(radius * radius)

    {
    }

    virtual ~SphereSnapshot()
    {
    }

    virtual void createBoundingBox() override
    {
		m_boundingBox = BoundingBox::createFromSphere(m_position, m_radius);
    }

    virtual void intersect(const Ray& ray, TraceResult& traceResult) const override
    {
		Vec3f l = m_position - ray.m_origin;
		float tca = l.dot(ray.m_direction);
		
		if (tca < 0.0f)
		{
			return;
		}

		float d2 = l.dot(l) - tca * tca;
		if (d2 > m_radiusSquared)
		{
			return;
		}

		float thc = sqrt(m_radiusSquared - d2);

		float frontHitDist = tca - thc;
		float backHitDist = tca + thc;

		// Inside the object, set the back hit as the front
		if (frontHitDist < 0.0f) frontHitDist = backHitDist;
		if (frontHitDist < traceResult.m_hitDistance)
		{
			traceResult.m_hitDistance = frontHitDist;
			traceResult.m_object = this;
			traceResult.m_hitOccured = true;
		}
    }

    float m_radius;
	float m_radiusSquared;
};

class Sphere : public Object
{
public:
	Sphere(const KeyFramedValue<bool>& activeKeyFrames, const KeyFramedValue<Vec3f>& positionKeyFrames, const Material& material, const KeyFramedValue<float>& radiusKeyFrames, bool octreeCompatible);

	virtual ObjectSnapshot* generateObjectSnapShotAtTime(float time) const override;

private:
	KeyFramedValue<float> m_radiusKeyFrames;
	float                 m_radiusSquared;
};
