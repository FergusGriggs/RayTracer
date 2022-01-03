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

	const DirectX::BoundingBox& GetBoundingCuboid() { return this->boundingCuboid; };
	bool AddToTree(T* entity, DirectX::BoundingBox boundingCuboid, bool forceAdd = false);
	void CullEmptyChildrenAndFilterTempPools();
	void Search(const DirectX::BoundingBox& boundingCuboid, std::vector<T*>* collidingEntities);
	void RaySearch(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDirection, std::vector<T*>* collidingEntities, float& closestDist);

	bool IsEmptyChild();
	void Clear();

private:
	void CreateChildren();

	DirectX::BoundingBox boundingCuboid;

	OctreeNode* m_root;
};
