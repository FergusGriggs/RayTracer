#pragma once

#include "../Vec3.h"

#include "../objects/Object.h"

#define OCTREE_MAX_TEMP_ENTITIES 3

class OctreeNode
{
public:
	OctreeNode();
	~OctreeNode();

	bool initialise(const BoundingBox& boundingBox);

	bool insertObject(ObjectSnapshot* object, bool forceAdd = false);
	void rayTrace(const Ray& ray, std::vector<ObjectSnapshot*>& collidingObjects);

	bool isEmpty();

	void cullEmptyChildrenAndFilterTempPools();

	void createChildren();
	void clear();

private:
	bool        m_hasChildren;
	bool        m_processed;

	BoundingBox m_boundingBox;

	std::vector<OctreeNode*> m_children;

	// Reserved for objects that cannot fit into any children,
	// or if no children exist once all objects have been added
	std::vector<ObjectSnapshot*> m_confirmedPool;

	// Filled with objects during tree construction
	std::vector<ObjectSnapshot*> m_tempPool;
};

