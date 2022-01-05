#pragma once

#include <vector>
#include <unordered_map>
#include <string.h>

#include "objects/object.h"
#include "octree/octree.h"
#include "ray.h"
#include "trace_result.h"

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void loadObjects(const std::string& sceneFilePath);
	void removeAllObjects();

	void addRandomSpheres(int count);
	void addRandomFallingSpheres(int count);

	void run();
	void runMultithreaded();

	void renderFrameAtTime(float time, int frameNumber, unsigned char* imageBuffer);

	Vec3f trace(const Ray& ray, int depth, std::vector<ObjectSnapshot*>& objects) const;
	Vec3f octreeTrace(const Ray& ray, int depth, Octree& octree) const;

private:
	unsigned int m_width = 640;
	unsigned int m_height = 480;

	int m_samplesPerPixel = 2;
	float m_animationDuration;

	bool m_cStyleWrite = false;

	bool m_useOctree = true;

	bool m_usingThreading = true;
	int m_numThreads = std::thread::hardware_concurrency() * 5;

	float m_timeIncreasePerFrame = 0.3f;

	std::vector<Object*>  m_objects;
	std::unordered_map<std::string, Material> m_materials;

	std::mutex m_outputMutex;

	unsigned char* m_imageBuffer;
};