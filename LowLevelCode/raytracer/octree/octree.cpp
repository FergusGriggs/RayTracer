#include "octree.h"

Octree::Octree()
{
}

Octree::~Octree()
{
}

void Octree::initialise(std::vector<Object*>& sceneObjects)
{
	BoundingBox sceneBoundingBox;
	for (int sceneObjectIndex = 0; sceneObjectIndex < sceneObjects.size(); ++sceneObjectIndex)
	{
		sceneBoundingBox.mergeWithBoundingBox(sceneObjects[sceneObjectIndex]->getBoundingBox());
	}

	m_root = new OctreeNode();
	m_root->initialise(sceneBoundingBox);

	for (int sceneObjectIndex = 0; sceneObjectIndex < sceneObjects.size(); ++sceneObjectIndex)
	{
		m_root->insertObject(sceneObjects[sceneObjectIndex]);
	}
}

void Octree::rayTrace(const Vec3f& rayOrigin, const Vec3f& rayDirection, std::vector<Object*>& collidingObjects)
{
	m_root->rayTrace(rayOrigin, rayDirection, collidingObjects);
}

void Octree::clear()
{
	m_root->clear();

	delete m_root;
}
