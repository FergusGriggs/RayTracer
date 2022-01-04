#include "octree.h"

Octree::Octree() :
	m_root(nullptr)
{
}

Octree::~Octree()
{
	clear();
}

void Octree::initialise(std::vector<ObjectSnapshot*>& sceneObjects)
{
	BoundingBox sceneBoundingBox;
	for (int sceneObjectIndex = 0; sceneObjectIndex < sceneObjects.size(); ++sceneObjectIndex)
	{
		sceneBoundingBox.mergeWithBoundingBox(sceneObjects[sceneObjectIndex]->m_boundingBox);
	}

	m_root = new OctreeNode();
	m_root->initialise(sceneBoundingBox);

	for (int sceneObjectIndex = 0; sceneObjectIndex < sceneObjects.size(); ++sceneObjectIndex)
	{
		m_root->insertObject(sceneObjects[sceneObjectIndex]);
	}

	m_potentialObjects.reserve(sceneObjects.size());
}

void Octree::rayTrace(const Ray& ray, Vec3)
{
	m_root->rayTrace(ray);
}

void Octree::clear()
{
	m_root->clear();

	delete m_root;
}
