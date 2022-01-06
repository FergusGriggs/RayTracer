#pragma once

#include <vector>
#include <unordered_map>
#include <string.h>

#include "objects/object.h"
#include "octree/octree.h"
#include "ray.h"
#include "trace_result.h"
#include "camera.h"
#include "../heap_manager.h"

enum class ThreadedMode
{
	eDisabled,
	eBatched,
	eContinual
};

#ifdef __unix
#include <pthread.h>
#include <thread>
#else
#include <thread>
#include <mutex> 
#endif

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

	bool m_cStyleWrite = true;

	bool m_useOctree = false;

	ThreadedMode m_threadedMode = ThreadedMode::eContinual;
	int m_numThreads = std::thread::hardware_concurrency() * 4;

	float m_timeIncreasePerFrame;

	std::vector<Object*>  m_objects;
	std::unordered_map<std::string, Material> m_materials;

	float m_fastestFrameTime = INFINITY;
	float m_slowestFrameTime = 0.0f;
	float m_totalFrameTimes = 0.0f;
	float m_animationRenderTime = 0.0f;

#ifdef __unix
	pthread_mutex_t m_outputMutex;
	pthread_mutex_t m_nextThreadTimeMutex;
	pthread_mutex_t m_threadStatsMutex;
#else
	std::mutex m_outputMutex;
	std::mutex m_nextThreadTimeMutex;
	std::mutex m_threadStatsMutex;
#endif

	unsigned char* m_imageBuffer;
};

#ifdef __unix
struct UnixThreadArgs
{
	Raytracer*     m_raytracer = nullptr;
	ThreadedMode   m_threadedMode = ThreadedMode::eContinual;
	int            m_threadID = 0;
	float          m_frameTime = 0.0f;
	int            m_frameNumber = 0;
	unsigned char* m_imageBuffer = nullptr;
};

static void * unixThreadRunner(void *voidArgs)
{
	UnixThreadArgs* args = (UnixThreadArgs*)voidArgs;
	if (args->m_threadedMode == ThreadedMode::eContinual)
	{
		args->m_raytracer->continualThreadLoop(args->m_threadID, args->m_imageBuffer);
	}
	else if (args->m_threadedMode == ThreadedMode::eBatched)
	{
		args->m_raytracer->renderFrameAtTime(args->m_frameTime, args->m_frameNumber, args->m_threadID, args->m_imageBuffer);
	}

	return nullptr;
}
#endif