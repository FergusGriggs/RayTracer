// [header]
// A very basic raytracer example.
// [/header]
// [compile]
// c++ -o raytracer -O3 -Wall raytracer.cpp
// [/compile]
// [ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// [/ignore]
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

#include <thread>
#include <mutex>  

#include <cstdio>

// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>

#include "rapidjson/document.h"

#include "objects/sphere.h"
#include "raytracer.h"
#include "math_defines.h"

#include "easings.h"
#include "static_functions.h"
#include "trace_result.h"
#include "timer.h"


//[comment]
// This variable controls the maximum recursion depth
//[/comment]
#define MAX_RAY_DEPTH 2

static float mix(float a, float b, float mix)
{
	return b * mix + a * (1 - mix);
}

static int getLineNumberFromOffset(std::string& string, size_t charOffset)
{
	int lineNumber = 0;
	size_t numIterations = std::min(string.length(), charOffset);
	for (size_t i = 0; i < numIterations; ++i)
	{
		if (string[i] == '\n') ++lineNumber;
	}

	return lineNumber;
}

static Vec3f getFloat3FromArray(rapidjson::Value::Array arrayObject)
{
	Vec3f returnValue = Vec3f(0.0f);
	int index = 0;
	for (auto itr = arrayObject.Begin(); itr != arrayObject.End(); ++itr)
	{
		if (index == 0) returnValue.x = itr->GetFloat();
		else if (index == 1) returnValue.y = itr->GetFloat();
		else if (index == 2) returnValue.z = itr->GetFloat();
		else return returnValue;

		++index;
	}

	return returnValue;
}

static void loadJsonValue(rapidjson::Value& jsonObject, const char* valueName, bool& value)
{
	value = jsonObject[valueName].GetBool();
}

static void loadJsonValue(rapidjson::Value& jsonObject, const char* valueName, float& value)
{
	value = jsonObject[valueName].GetFloat();
}

static void loadJsonValue(rapidjson::Value& jsonObject, const char* valueName, Vec3f& value)
{
	value = getFloat3FromArray(jsonObject[valueName].GetArray());
}

template<class T>
static void loadValueKeyFrames(KeyFramedValue<T>& valueKeyFrames, rapidjson::Value& jsonObject, const char* keyFrameNames, const T& defaultValue)
{
	if (jsonObject.HasMember(keyFrameNames))
	{
		rapidjson::Value& keyFramesArray = jsonObject[keyFrameNames];

		if (keyFramesArray.IsArray())
		{
			for (auto keyFramesArrayItr = keyFramesArray.Begin(); keyFramesArrayItr != keyFramesArray.End(); ++keyFramesArrayItr)
			{
				T value;
				loadJsonValue((*keyFramesArrayItr), "value", value);

				float time = (*keyFramesArrayItr)["time"].GetFloat();

				EaseType easeIn = EaseType::eUnset;
				EaseType easeOut = EaseType::eUnset;

				if ((*keyFramesArrayItr).HasMember("ease_in"))
				{
					easeIn = getEaseTypeFromString((*keyFramesArrayItr)["ease_in"].GetString());
				}

				if ((*keyFramesArrayItr).HasMember("ease_out"))
				{
					easeOut = getEaseTypeFromString((*keyFramesArrayItr)["ease_out"].GetString());
				}

				valueKeyFrames.addKeyFrame(KeyFrame<T>(value, time, easeIn, easeOut));
			}
		}
	}
	valueKeyFrames.processKeyFrames(defaultValue);
}

Raytracer::Raytracer()
{
	srand(13);

	std::string filePath = "raytracer/scenes/test.json";
	loadObjects(filePath);

	Timer animationTimer;
	
	if (m_usingThreading)
	{
		runMultithreaded();
	}
	else
	{
		run();
	}

	animationTimer.tock();
	std::cout << "Finished Animation [Took " << static_cast<float>(animationTimer.duration().count()) * 0.000001f << "s]\n";
}

Raytracer::~Raytracer()
{
}

void Raytracer::loadObjects(const std::string& sceneFilePath)
{
	std::ifstream sceneFileStream(sceneFilePath.c_str());

	if (!sceneFileStream) return;

	std::stringstream buffer;
	buffer << sceneFileStream.rdbuf();
	std::string sceneFileStr = buffer.str();

	rapidjson::Document document;
	document.Parse(sceneFileStr.c_str());
	if (document.HasParseError())
	{
		rapidjson::ParseErrorCode error = document.GetParseError();
		size_t errorOffset = document.GetErrorOffset();

		int errorStart = std::max(0, static_cast<int>(errorOffset) - 20);
		int errorEnd = std::min(strlen(sceneFileStr.c_str()), errorOffset + 20);

		int beforeLen = errorOffset - errorStart;
		int afterLen = errorEnd - errorOffset;

		std::string errorString = sceneFileStr.substr(errorStart, beforeLen) + "[HERE]" + sceneFileStr.substr(errorOffset, afterLen);
		errorString.erase(std::remove(errorString.begin(), errorString.end(), '\n'), errorString.end());
		errorString.erase(std::remove(errorString.begin(), errorString.end(), '\t'), errorString.end());

		int lineNumber = getLineNumberFromOffset(sceneFileStr, errorOffset);

		std::string finalErrorString = "Parsing Scene JSON\nCode: '" + std::to_string(error) + "'\nLine Num: '" + std::to_string(lineNumber) + "'\nError String: '" + errorString + "'\n";

		std::cout << finalErrorString << std::endl;
		return;
	}

	if (document.HasMember("Scene"))
	{
		rapidjson::Value& sceneObject = document["Scene"];


		if (sceneObject.HasMember("MetaData"))
		{
			rapidjson::Value& sceneMetaData = sceneObject["MetaData"];

			if (sceneMetaData.HasMember("animation_duration"))
			{
				m_animationDuration = sceneMetaData["animation_duration"].GetFloat();
			}
			if (sceneMetaData.HasMember("time_increase_per_frame"))
			{
				m_timeIncreasePerFrame = sceneMetaData["time_increase_per_frame"].GetFloat();
			}
			
		}

		if (sceneObject.HasMember("Materials"))
		{
			rapidjson::Value& materialListObj = sceneObject["Materials"];

			if (materialListObj.IsArray())
			{
				for (auto materialItr = materialListObj.Begin(); materialItr != materialListObj.End(); ++materialItr)
				{
					std::string materialId = (*materialItr)["id"].GetString();

					Vec3f baseColour = getFloat3FromArray((*materialItr)["base_colour"].GetArray());
					Vec3f emissiveColour = getFloat3FromArray((*materialItr)["emissive_colour"].GetArray());

					float roughness = (*materialItr)["roughness"].GetFloat();
					float metallic = (*materialItr)["metallic"].GetFloat();
					float transmission = (*materialItr)["transmission"].GetFloat();

					Material material(baseColour, roughness, metallic, transmission, emissiveColour);
					m_materials.insert({ materialId , material});
				}
			}
		}

		if (sceneObject.HasMember("Objects"))
		{
			rapidjson::Value& objects = sceneObject["Objects"];

			if (objects.IsArray())
			{
				for (auto objectItr = objects.Begin(); objectItr != objects.End(); ++objectItr)
				{
					std::string type = (*objectItr)["type"].GetString();
					std::string materialId = (*objectItr)["material_id"].GetString();

					bool octreeCompatible = true;
					if (objectItr->HasMember("octree_compatible"))
					{
						octreeCompatible = (*objectItr)["octree_compatible"].GetBool();
					}
					
					KeyFramedValue<bool>  activeKeyFrames;
					loadValueKeyFrames(activeKeyFrames, (*objectItr), "active_keyframes", true);

					KeyFramedValue<Vec3f>  positionKeyFrames;
					loadValueKeyFrames(positionKeyFrames, (*objectItr), "position_keyframes", Vec3f(0.0f, 0.0f, 0.0f));

					KeyFramedValue<float>  radiusKeyFrames;
					loadValueKeyFrames(radiusKeyFrames, (*objectItr), "radius_keyframes", 1.0f);

					Sphere* sphere = new Sphere(activeKeyFrames, positionKeyFrames, m_materials[materialId], radiusKeyFrames, octreeCompatible);
					m_objects.push_back(sphere);
				}
			}
		}
	}

	addRandomFallingSpheres(500);
}

void Raytracer::removeAllObjects()
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		delete m_objects[i];
	}
	m_objects.clear();
}

void Raytracer::addRandomSpheres(int count)
{
	Vec3f minSpawnPos(-20.0f, -4.0f, -70.0f);
	Vec3f maxSpawnPos(20.0f, 10.0f, -40.0f);

	auto materialItr = m_materials.begin();
	for (int sphereIndex = 0; sphereIndex < count; ++sphereIndex)
	{
		KeyFramedValue<bool> activeKeyFrames;
		activeKeyFrames.addKeyFrame(KeyFrame<bool>(true, 0.0f));

		KeyFramedValue<Vec3f> positionKeyFrames;
		positionKeyFrames.addKeyFrame(KeyFrame<Vec3f>(Vec3f::randomInBox(minSpawnPos, maxSpawnPos), 0.0f));
		positionKeyFrames.addKeyFrame(KeyFrame<Vec3f>(Vec3f::randomInBox(minSpawnPos, maxSpawnPos), 100.0f));

		KeyFramedValue<float> radiusKeyFrames;
		radiusKeyFrames.addKeyFrame(KeyFrame<float>(static_functions::randomRange(0.25f, 1.0f), 0.0f));

		m_objects.push_back(new Sphere(activeKeyFrames, positionKeyFrames, materialItr->second, radiusKeyFrames, true));

		if (++materialItr == m_materials.end())
		{
			materialItr = m_materials.begin();
		}
	}
}

void Raytracer::addRandomFallingSpheres(int count)
{
	Vec3f minSpawnPos(-20.0f, 40.0f, -70.0f);
	Vec3f maxSpawnPos(20.0f, 40.0f, -30.0f);

	auto materialItr = m_materials.begin();
	for (int sphereIndex = 0; sphereIndex < count; ++sphereIndex)
	{
		float startTime = static_functions::randomRange(0.5f, 50.0f);

		KeyFramedValue<bool> activeKeyFrames;
		activeKeyFrames.addKeyFrame(KeyFrame<bool>(false, 0.0f));
		activeKeyFrames.addKeyFrame(KeyFrame<bool>(true, startTime));

		KeyFramedValue<float> radiusKeyFrames;
		float radius = static_functions::randomRange(0.25f, 1.0f);
		radiusKeyFrames.addKeyFrame(KeyFrame<float>(radius, 0.0f));

		KeyFramedValue<Vec3f> positionKeyFrames;
		Vec3f startPosition = Vec3f::randomInBox(minSpawnPos, maxSpawnPos);
		Vec3f endPosition = Vec3f(startPosition.x, -4.0f + radius, startPosition.z);
		
		positionKeyFrames.addKeyFrame(KeyFrame<Vec3f>(startPosition, startTime));
		positionKeyFrames.addKeyFrame(KeyFrame<Vec3f>(endPosition, startTime + 50.0, EaseType::eBounce, EaseType::eUnset));

		m_objects.push_back(new Sphere(activeKeyFrames, positionKeyFrames, materialItr->second, radiusKeyFrames, true));

		if (++materialItr == m_materials.end())
		{
			materialItr = m_materials.begin();
		}
	}
}

//[comment]
// This is the main trace function. It takes a ray as argument (defined by its origin
// and direction). We test if this ray intersects any of the geometry in the scene.
// If the ray intersects an object, we compute the intersection point, the normal
// at the intersection point, and shade this point using this information.
// Shading depends on the surface property (is it transparent, reflective, diffuse).
// The function returns a color for the ray. If the ray intersects an object that
// is the color of the object at the intersection point, otherwise it returns
// the background color.
//[/comment]
Vec3f Raytracer::trace(const Ray &ray, int depth, std::vector<ObjectSnapshot*>& objects) const
{
	TraceResult traceResult;

	// Find intersection of this ray with an object in the scene
	for (int objectIndex = 0; objectIndex < objects.size(); ++objectIndex)
	{
		objects[objectIndex]->intersect(ray, traceResult);
	}

	// if there's no intersection return a sky gradient
	if (!traceResult.m_hitOccured)
	{
		float t = 0.5f * (ray.m_direction.y + 1.0f);
		return Vec3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3f(0.5f, 0.7f, 1.0f) * t;
	}

	traceResult.m_hitPoint = ray.m_origin + ray.m_direction * traceResult.m_hitDistance;
	traceResult.m_hitNormal = Vec3f::normalise(traceResult.m_hitPoint - traceResult.m_object->m_position);

	// If the normal and the view direction are not opposite to each other
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.

	// Move the intersection away from the surface slightly to avoid self collision
	float bias = 1e-3f;
	traceResult.m_hitPoint += traceResult.m_hitNormal * bias;

	// Flip the normal if we're inside the object
	bool inside = false;
	if (ray.m_direction.dot(traceResult.m_hitNormal) > 0.0f)
	{
		traceResult.m_hitNormal = -traceResult.m_hitNormal;
		inside = true;
	}

	// Shade and scatter another ray
	if (depth < MAX_RAY_DEPTH)
	{
		Ray scattered;
		Vec3f attenuation;
		if (traceResult.m_object->m_material.scatter(ray, traceResult, attenuation, scattered))
		{
			return attenuation * trace(scattered, depth + 1, objects);
		}
	}

	return Vec3f(0.0f);
}

Vec3f Raytracer::octreeTrace(const Ray& ray, int depth, Octree& octree) const
{
	TraceResult traceResult;

	octree.rayTrace(ray, traceResult);

	// if there's no intersection return a sky gradient
	if (!traceResult.m_hitOccured)
	{
		float t = 0.5f * (ray.m_direction.y + 1.0f);
		return Vec3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3f(0.5f, 0.7f, 1.0f) * t;
	}

	traceResult.m_hitPoint = ray.m_origin + ray.m_direction * traceResult.m_hitDistance;
	traceResult.m_hitNormal = Vec3f::normalise(traceResult.m_hitPoint - traceResult.m_object->m_position);

	// Move the intersection away from the surface slightly to avoid self collision
	float bias = 1e-3f;
	traceResult.m_hitPoint += traceResult.m_hitNormal * bias;

	// Flip the normal if we're inside the object
	bool inside = false;
	if (ray.m_direction.dot(traceResult.m_hitNormal) > 0.0f)
	{
		traceResult.m_hitNormal = -traceResult.m_hitNormal;
		inside = true;
	}

	// Shade and scatter another ray
	if (depth < MAX_RAY_DEPTH)
	{
		Ray scattered;
		Vec3f attenuation;
		if (traceResult.m_object->m_material.scatter(ray, traceResult, attenuation, scattered))
		{
			return attenuation * octreeTrace(scattered, depth + 1, octree);
		}
	}

	return Vec3f(0.0f);
}

void Raytracer::run()
{
	m_imageBuffer = new unsigned char[m_width * m_height * 3];

	float nextFrameTime = 0.0f;
	int frameNumber = 0;

	while (nextFrameTime <= m_animationDuration)
	{
		renderFrameAtTime(nextFrameTime, frameNumber++, m_imageBuffer);

		nextFrameTime += m_timeIncreasePerFrame;
	}

	delete[] m_imageBuffer;
}

void Raytracer::runMultithreaded()
{
	std::vector<std::thread*> threads;
	std::vector<unsigned char*> imageBuffers;

	for (int i = 0; i < m_numThreads; ++i)
	{
		imageBuffers.push_back(new unsigned char[m_width * m_height * 3]);
	}

	bool done = false;
	float nextFrameTime = 0.0f;
	int frameNumber = 0;

	while (!done)
	{
		m_outputMutex.lock();
		std::cout << "Starting new thread batch\n";
		m_outputMutex.unlock();

		for (int i = 0; i < m_numThreads; ++i)
		{
			if (nextFrameTime <= m_animationDuration)
			{
				threads.push_back(new std::thread(&Raytracer::renderFrameAtTime, this, nextFrameTime, frameNumber++, imageBuffers[threads.size()]));
				//threads.push_back(new std::thread(Raytracer::test, nextFrameTime));

				nextFrameTime += m_timeIncreasePerFrame;
			}
			else
			{
				done = true;
				break;
			}
		}

		for (int threadIndex = 0; threadIndex < threads.size(); ++threadIndex)
		{
			threads[threadIndex]->join();
			delete threads[threadIndex];
		}

		threads.clear();
	}

	for (int i = 0; i < m_numThreads; ++i)
	{
		delete[] imageBuffers[i];
	}

	imageBuffers.clear();
}


//[comment]
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
//[/comment]
void Raytracer::renderFrameAtTime(float time, int frameNumber, unsigned char* imageBuffer)
{
	Timer frameTimer;

	std::vector<ObjectSnapshot*> allFrameObjects;
	for (int objectIndex = 0; objectIndex < m_objects.size(); ++objectIndex)
	{
		ObjectSnapshot* objectSnapshot = m_objects[objectIndex]->generateObjectSnapShotAtTime(time);

		if (objectSnapshot != nullptr)
		{
			objectSnapshot->createBoundingBox();
			allFrameObjects.push_back(objectSnapshot);
		}
	}

	Octree octree;
	if (m_useOctree)
	{
		octree.initialise(allFrameObjects);
	}

	unsigned char* pixel = imageBuffer;

	float invWidth = 1.0f / float(m_width), invHeight = 1.0f / float(m_height);
	float fov = 30.0f, aspectratio = m_width / float(m_height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);

	// Trace rays
	for (float y = 0.0f; y < m_height; ++y)
	{
		for (float x = 0.0f; x < m_width; ++x)
		{
			Vec3f pixelColour;

			for (int sampleIndex = 0; sampleIndex < m_samplesPerPixel; ++sampleIndex)
			{
				float u = x + static_functions::randomNegativeOneToOne() * 0.5f;
				float v = y + static_functions::randomNegativeOneToOne() * 0.5f;

				float xx = (2.0f * ((u + 0.5f) * invWidth) - 1.0f) * angle * aspectratio;
				float yy = (1.0f - 2.0f * ((v + 0.5f) * invHeight)) * angle;
				Vec3f raydir(xx, yy, -1.0f);
				raydir.normalize();

				Ray ray(Vec3f(0.0f), raydir);

				if (m_useOctree)
				{
					pixelColour += octreeTrace(ray, 0, octree);
				}
				else
				{
					pixelColour += trace(ray, 0, allFrameObjects);
				}
			}

			float scale = 1.0f / float(m_samplesPerPixel);
			pixelColour.x = sqrt(scale * pixelColour.x);
			pixelColour.y = sqrt(scale * pixelColour.y);
			pixelColour.z = sqrt(scale * pixelColour.z);

			*(pixel++) = pixelColour.x * 255.0f;
			*(pixel++) = pixelColour.y * 255.0f;
			*(pixel++) = pixelColour.z * 255.0f;
		}
	}

	for (int objectIndex = 0; objectIndex < allFrameObjects.size(); ++objectIndex)
	{
		delete allFrameObjects[objectIndex];
	}
	
	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::stringstream ss;
	ss << "raytracer/output/spheres" << frameNumber << ".ppm";

	std::string tempString = ss.str();
	char* filename = (char*)tempString.c_str();

	// Optional c-style writing to file
	if (m_cStyleWrite)
	{
		std::FILE* file;
		fopen_s(&file, filename, "wb");

		if (file)
		{
			std::stringstream metaDataSS;
			metaDataSS << "P6\n" << m_width << " " << m_height << "\n255\n";
			std::string metaDataString = metaDataSS.str();

			std::fwrite(metaDataString.c_str(), sizeof(unsigned char), metaDataString.size(), file);
			std::fwrite(imageBuffer, sizeof(unsigned char), m_width * m_height * 3, file);
			std::fclose(file);
		}
	}
	else
	{
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << m_width << " " << m_height << "\n255\n";
		for (unsigned i = 0; i < m_width * m_height * 3; ++i)
		{
			ofs << imageBuffer[i];
		}
		ofs.close();
	}

	frameTimer.tock();

	m_outputMutex.lock();
	std::cout << "Rendered Frame " << frameNumber << " [Took " << static_cast<float>(frameTimer.duration().count()) * 0.000001f << "s] [" << (time / m_animationDuration) * 100.0f << "% Done]\n";
	m_outputMutex.unlock();
}
