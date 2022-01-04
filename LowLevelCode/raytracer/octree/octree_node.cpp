#include "octree_node.h"

OctreeNode::OctreeNode() :
	m_hasChildren(false),
	m_processed(false)
{
}

OctreeNode::~OctreeNode()
{
}

bool OctreeNode::initialise(const BoundingBox& boundingBox)
{
	m_boundingBox = boundingBox;

	return true;
}

bool OctreeNode::insertObject(ObjectSnapshot* object, bool forceAdd)
{
	// If contained in its cuboid and the octree has not been finalized/processed (empty children are deleted and may break adding operatons)
	if (forceAdd || (m_boundingBox.contains(object->m_boundingBox) && !m_processed))
	{
		if (m_hasChildren)
		{
			// If it has children attempt to fit it in any of the children
			for (int i = 0; i < 8; ++i)
			{
				if (m_children[i]->insertObject(object))
				{
					return true;
				}
			}

			// If it fits in the parent cuboid, but not in a child cuboid, add to perm pool
			m_confirmedPool.push_back(object);
			return true;
		}

		// If the number of temp entities exceeds the max, create children and filter
		// temp entities into the children and perm pool

		if (m_tempPool.size() < OCTREE_MAX_TEMP_ENTITIES)
		{
			// If it fits in the parent cuboid but children do not exist and the max temp entities hasn't been reached, add to temp pool
			m_tempPool.push_back(object);
			return true;
		}
		else
		{
			this->createChildren();

			// Empty temp pool into children
			for (int j = 0; j < m_tempPool.size(); ++j)
			{
				bool placedInChild = false;
				for (int i = 0; i < 8; ++i)
				{
					if (this->m_children[i]->insertObject(object))
					{
						placedInChild = true;
						break;
					}
				}

				if (!placedInChild)
				{
					m_tempPool.push_back(m_tempPool.at(j));
				}
			}

			// Clear the temp pool now it's entities have been filtered out
			m_tempPool.clear();

			for (int i = 0; i < 8; ++i)
			{
				if (m_children[i]->insertObject(object))
				{
					return true;
				}
			}

			// Doesn't fit into created children, so add to perm pool
			m_confirmedPool.push_back(object);
			return true;
		}
	}

	return false;
}

void OctreeNode::cullEmptyChildrenAndFilterTempPools()
{
	m_confirmedPool.insert(m_confirmedPool.end(), m_tempPool.begin(), m_tempPool.end());
	m_tempPool.clear();

	if (m_hasChildren)
	{
		for (int i = 7; i >= 0; --i)
		{
			m_children[i]->cullEmptyChildrenAndFilterTempPools();

			if (m_children[i]->isEmpty())
			{
				m_children.erase(m_children.begin() + i);
			}
		}

		if (m_children.size() == 0)
		{
			m_hasChildren = false;
		}
	}

	m_processed = true;
}

void OctreeNode::rayTrace(const Ray& ray, std::vector<ObjectSnapshot*>& collidingObjects)
{
	Vec3f invDir = Vec3f(0.0f);

	invDir.x = 1.0f / ray.m_origin.x;
	invDir.y = 1.0f / ray.m_origin.y;
	invDir.z = 1.0f / ray.m_origin.z;

	if (m_boundingBox.intersects(ray, invDir))
	{
		for (int i = 0; i < m_confirmedPool.size(); ++i)
		{
			if (m_confirmedPool[i]->m_boundingBox.intersects(ray, invDir))
			{
				collidingObjects.push_back(m_confirmedPool[i]);
			}
		}

		if (m_hasChildren)
		{
			for (int i = 0; i < m_children.size(); ++i)
			{
				m_children[i]->rayTrace(ray, collidingObjects);
			}
		}
	}
}

bool OctreeNode::isEmpty()
{
	if (m_hasChildren)
	{
		return false;
	}

	if (m_confirmedPool.size() > 0)
	{
		return false;
	}

	if (m_tempPool.size() > 0)
	{
		return false;
	}

	return true;
}

void OctreeNode::clear()
{
	if (m_hasChildren)
	{
		m_hasChildren = false;

		for (int i = 0; i < 8; ++i)
		{
			m_children[i]->clear();

			delete m_children[i];
		}
	}
}

void OctreeNode::createChildren()
{
	if (!m_children.empty())
	{
		return;
	}

	m_children.reserve(8);

	Vec3f halfSize = m_boundingBox.m_extents * 0.5f;
	for (int xSegment = 0; xSegment < 2; ++xSegment)
	{
		for (int ySegment = 0; ySegment < 2; ++ySegment)
		{
			for (int zSegment = 0; zSegment < 2; ++zSegment)
			{
				m_children.push_back(new OctreeNode());
				OctreeNode* newChild = m_children.back();

				Vec3f min = m_boundingBox.m_minPoint + Vec3f((float)xSegment * halfSize.x, (float)ySegment * halfSize.y, (float)zSegment * halfSize.z);
				Vec3f max = min + halfSize;

				newChild->initialise(BoundingBox(min, max));
			}
		}
	}

	m_hasChildren = true;
}