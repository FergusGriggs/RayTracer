#pragma once

#include <vector>
#include <unordered_map>
#include <string.h>

#include "objects/Object.h"

class Raytracer
{
public:
	Raytracer();
	~Raytracer();

	void loadObjects(const std::string& sceneFilePath);
	void removeAllObjects();

	void updateAllObjects();

	void basicRender();
	void smoothScaling();
	void simpleShrinking();

	void run();

	void renderFrame();

	Vec3f trace(const Vec3f& rayOrigin, const Vec3f& rayDir, int depth) const;

private:
	unsigned int m_width = 640;
	unsigned int m_height = 480;

	int m_currentFrame = 0;

	std::vector<Object*>  m_objects;
	std::unordered_map<std::string, Material> m_materials;
};