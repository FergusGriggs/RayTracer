#pragma once

#include "../Vec3.h"

#include "../objects/Object.h"

class OctreeNode
{
public:
	OctreeNode(const Vec3f& topLeft, const Vec3f& extents);
	~OctreeNode();

private:
	std::vector<OctreeNode*> m_childrenNodes;

	// Reserved for objects that cannot fit into any children,
	// or if no children exist once all objects have been added
	std::vector<Object*>     m_confirmedPool;

	// Filled with objects during tree construction
	std::vector<Object*>     m_tempPool; 
};

