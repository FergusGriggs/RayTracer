#pragma once

#include "Vec3.h"

class BoundingBox
{
public:
	BoundingBox(const Vec3f& minPoint, const Vec3f& extents);
	~BoundingBox();

	bool intersects(const BoundingBox& other);
	bool contains(const BoundingBox& other);

	static BoundingBox createFromSphere(const Vec3f& position, float radius);
	static void        mergeBoundingBoxIntoFirst(BoundingBox& first, const BoundingBox& other);

private:
	Vec3f m_minPoint;
	Vec3f m_extents;

};

