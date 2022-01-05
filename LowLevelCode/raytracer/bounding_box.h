#pragma once

#include "vec3.h"
#include "ray.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Vec3f& minPoint, const Vec3f& extents);
	~BoundingBox();

	bool intersects(const BoundingBox& other) const;
	bool intersects(const Ray& ray, const Vec3f& invDir) const;

	bool contains(const BoundingBox& other) const;

	void mergeWithBoundingBox(const BoundingBox& other);

	static BoundingBox createFromSphere(const Vec3f& position, float radius);

public:
	Vec3f m_minPoint;
	Vec3f m_extents;
};

