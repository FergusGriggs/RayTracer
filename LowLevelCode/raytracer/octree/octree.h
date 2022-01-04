#pragma once

#include <vector>

#include "octree_node.h"

#include "../objects/Object.h"

class Octree
{
public:
	Octree();
	~Octree();

	void initialise(std::vector<Object*>& sceneObjects);
	void rayTrace(const Vec3f& rayOrigin, const Vec3f& rayDirection, std::vector<Object*>& collidingObjects);

	void clear();

private:
	OctreeNode* m_root;
};
