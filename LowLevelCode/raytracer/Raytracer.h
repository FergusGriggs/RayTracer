#pragma once

#include <vector>
#include <unordered_map>
#include <string.h>

#include "objects/Object.h"
#include "octree/octree.h"
#include "ray.h"

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void loadObjects(const std::string& sceneFilePath);
	void removeAllObjects();

	void run();

	void renderFrameAtTime(float time);

	Vec3f trace(const Ray& ray, int depth, std::vector<ObjectSnapshot*>& objects) const;
	Vec3f octreeTrace(const Ray& ray, int depth, Octree& octree) const;

private:
	unsigned int m_width = 640;
	unsigned int m_height = 480;

	int m_frameRate = 0;
	int m_frameCount = 0;
	int m_samplesPerPixel = 4;

	bool m_useOctree = true;

	std::vector<Object*>  m_objects;
	std::unordered_map<std::string, Material> m_materials;

	Vec3f* m_imageBuffer;
};