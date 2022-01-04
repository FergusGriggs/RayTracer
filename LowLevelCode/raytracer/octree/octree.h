#pragma once

#include <vector>

#include "octree_node.h"

#include "../objects/Object.h"

class Octree
{
public:
	Octree();
	~Octree();

	void initialise(std::vector<ObjectSnapshot*>& sceneObjects);
	void rayTrace(const Ray& ray, float& t0, float& t1);

	void clear();

private:
	OctreeNode* m_root;

	std::vector<ObjectSnapshot*> m_potentialObjects;
};
