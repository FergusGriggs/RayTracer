#pragma once

#include <vector>

#include "octree_node.h"

#include "../objects/Object.h"
#include "../trace_result.h"

class Octree
{
public:
	Octree();
	~Octree();

	void initialise(std::vector<ObjectSnapshot*>& sceneObjects);
	void rayTrace(const Ray& ray, TraceResult& traceResult);

	void clear();

private:
	OctreeNode* m_root;

	std::vector<ObjectSnapshot*> m_externalObjects;

	std::vector<ObjectSnapshot*> m_traceObjects;
};
