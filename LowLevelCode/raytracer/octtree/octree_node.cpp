#include "octree_node.h"

OctreeNode::OctreeNode(const Vec3f& topLeft, const Vec3f& extents) :
	m_topLeft(topLeft),
	m_extents(extents)
{
}

OctreeNode::~OctreeNode()
{
}
