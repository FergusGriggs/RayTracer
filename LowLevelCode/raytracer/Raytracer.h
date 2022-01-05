#pragma once

#include <vector>
#include <unordered_map>
#include <string.h>

#include "objects/object.h"
#include "octree/octree.h"
#include "ray.h"
#include "trace_result.h"
#include "camera.h"

enum class ThreadedMode
{
	eDisabled,
	eBatched,
	eContinual
};

static std::string getThreadedModeString(ThreadedMode mode)
{
	switch (mode)
	{
	case ThreadedMode::eDisabled:
		return "Disabled";
	case ThreadedMode::eContinual:
		return "Continual";
	case ThreadedMode::eBatched:
		return "Batched";
	}
	return "?";
}

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void loadObjects(const std::string& sceneFilePath);
	void removeAllObjects();

	void addRandomSpheres(int count, const BoundingBox& spawnBox);
	void addRandomFallingSpheres(int count, const BoundingBox& spawnBox);

	void run();
	void runMultithreaded();

	void renderFrameAtTime(float time, int frameNumber, int threadID, unsigned char* imageBuffer);
	void continualThreadLoop(int threadID, unsigned char* imageBuffer);
	void getNextFrameAndTime(int& frame, float& frameTime);

	void addFrameStats(float timeToComplete);
	void printRenderStats() const;

	Vec3f trace(const Ray& ray, int depth, std::vector<ObjectSnapshot*>& objects) const;
	Vec3f octreeTrace(const Ray& ray, int depth, Octree& octree) const;

private:
	unsigned int m_width = 640;
	unsigned int m_height = 480;

	Camera m_camera = Camera(Vec3f(0.0f, 4.0f, 80.0f), Vec3f(0.0f, -2.0f, -25.0f), 7.5f, (float)m_width / (float)m_height, 0.4f, -1.0f);

	float m_nextFrameTime;
	int   m_nextFrame;

	int m_samplesPerPixel = 4;
	int m_maxBounces = 5;
	float m_animationDuration;

	bool m_cStyleWrite = false;

	bool m_useOctree = true;

	ThreadedMode m_threadedMode = ThreadedMode::eBatched;
	int m_numThreads = std::thread::hardware_concurrency() * 4;

	float m_timeIncreasePerFrame;

	std::vector<Object*>  m_objects;
	std::unordered_map<std::string, Material> m_materials;

	float m_fastestFrameTime = INFINITY;
	float m_slowestFrameTime = 0.0f;
	float m_totalFrameTimes = 0.0f;
	float m_animationRenderTime = 0.0f;

	std::mutex m_outputMutex;
	std::mutex m_nextThreadTimeMutex;
	std::mutex m_threadStatsMutex;

	unsigned char* m_imageBuffer;
};