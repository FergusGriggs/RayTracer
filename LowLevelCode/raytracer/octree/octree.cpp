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
		if (sceneObjects[sceneObjectIndex]->m_octreeCompatible)
		{
			sceneBoundingBox.mergeWithBoundingBox(sceneObjects[sceneObjectIndex]->m_boundingBox);
		}
	}

	// Inflate bounding box slightly
	sceneBoundingBox.m_minPoint -= Vec3f(0.1f);
	sceneBoundingBox.m_extents += Vec3f(0.2f);

	m_root = new OctreeNode();
	m_root->initialise(sceneBoundingBox);

	for (int sceneObjectIndex = 0; sceneObjectIndex < sceneObjects.size(); ++sceneObjectIndex)
	{
		if (sceneObjects[sceneObjectIndex]->m_octreeCompatible)
		{
			m_root->insertObject(sceneObjects[sceneObjectIndex]);
		}
		else
		{
			m_externalObjects.push_back(sceneObjects[sceneObjectIndex]);
		}
	}

	m_root->cullEmptyChildrenAndFilterTempPools();

	// Reserve some space in advance for the maximum number of objects that could be traced
	m_traceObjects.reserve(sceneObjects.size());
}

void Octree::rayTrace(const Ray& ray, TraceResult& traceResult)
{
	// Check collision against objects excluded from the octree
	for (int externalObjectIndex = 0; externalObjectIndex < m_externalObjects.size(); ++externalObjectIndex)
	{
		m_externalObjects[externalObjectIndex]->intersect(ray, traceResult);
	}

	// Retrieve all the objects in the confirmed pools of the octree nodes that the ray passed through
	m_traceObjects.clear();
	m_root->rayTrace(ray, m_traceObjects);

	// Test for collision against those objects
	for (int traceObjectIndex = 0; traceObjectIndex < m_traceObjects.size(); ++traceObjectIndex)
	{
		m_traceObjects[traceObjectIndex]->intersect(ray, traceResult);
	}
}

void Octree::clear()
{
	if (m_root != nullptr)
	{
		m_root->clear();

		delete m_root;
	}
}
