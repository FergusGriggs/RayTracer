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

// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>

#include "rapidjson/document.h"

#include "objects/Sphere.h"
#include "Raytracer.h"
#include "MathDefines.h"

#include "easings.h"
#include "static_functions.h"

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
					auto easeStringsItr = sc_easeTypeStrings.find((*keyFramesArrayItr)["ease_in"].GetString());
					if (easeStringsItr != sc_easeTypeStrings.end())
					{
						easeIn = easeStringsItr->second;
					}
				}

				if ((*keyFramesArrayItr).HasMember("ease_out"))
				{
					auto easeStringsItr = sc_easeTypeStrings.find((*keyFramesArrayItr)["ease_out"].GetString());
					if (easeStringsItr != sc_easeTypeStrings.end())
					{
						easeOut = easeStringsItr->second;
					}
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
	run();
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

			if (sceneMetaData.HasMember("frame_rate"))
			{
				m_frameRate = sceneMetaData["frame_rate"].GetInt();
			}
			if (sceneMetaData.HasMember("frame_count"))
			{
				m_frameCount = sceneMetaData["frame_count"].GetInt();
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

					Vec3f baseColour = getFloat3FromArray((*materialItr)["baseColour"].GetArray());
					Vec3f emissiveColour = getFloat3FromArray((*materialItr)["emissiveColour"].GetArray());

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
					std::string materialId = (*objectItr)["materialId"].GetString();

					KeyFramedValue<bool>  activeKeyFrames;
					loadValueKeyFrames(activeKeyFrames, (*objectItr), "active_keyframes", true);

					KeyFramedValue<Vec3f>  positionKeyFrames;
					loadValueKeyFrames(positionKeyFrames, (*objectItr), "position_keyframes", Vec3f(0.0f, 0.0f, 0.0f));

					KeyFramedValue<float>  radiusKeyFrames;
					loadValueKeyFrames(radiusKeyFrames, (*objectItr), "radius_keyframes", 1.0f);

					Sphere* sphere = new Sphere(activeKeyFrames, positionKeyFrames, m_materials[materialId], radiusKeyFrames);
					m_objects.push_back(sphere);
				}
			}
		}
	}
}

void Raytracer::removeAllObjects()
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		delete m_objects[i];
	}
	m_objects.clear();
}

void Raytracer::updateAllObjects()
{
	for (auto& object : m_objects)
	{
		object->update(static_cast<float>(m_currentFrame));
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
Vec3f Raytracer::trace(const Ray &ray, int depth) const
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	float tnear = INFINITY;
	const Object* object = nullptr;
	// find intersection of this ray with the sphere in the scene
	for (unsigned i = 0; i < m_objects.size(); ++i)
	{
		float t0 = INFINITY, t1 = INFINITY;
		if (m_objects[i]->isActive())
		{
			if (m_objects[i]->intersect(ray, t0, t1))
			{
				if (t0 < 0.00001f) t0 = t1;
				if (t0 < tnear)
				{
					tnear = t0;
					object = m_objects[i];
				}
			}
		}
	}
	// if there's no intersection return a sky gradient
	if (object == nullptr)
	{
		float t = 0.5f * (ray.m_direction.y + 1.0f);
		return Vec3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3f(0.5f, 0.7f, 1.0f) * t;
	}

	const Material& objectMaterial = object->getMaterial();

	Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
	Vec3f intersectPoint = ray.m_origin + ray.m_direction * tnear; // point of intersection
	Vec3f intersectNormal = intersectPoint - object->getPosition(); // normal at the intersection point
	intersectNormal.normalize(); // normalize normal direction
					             // If the normal and the view direction are not opposite to each other
					             // reverse the normal direction. That also means we are inside the sphere so set
					             // the inside bool to true. Finally reverse the sign of IdotN which we want
					             // positive.
	float bias = 1e-3f; // add some bias to the point from which we will be tracing
	bool inside = false;

	if (ray.m_direction.dot(intersectNormal) > 0.0f)
	{
		intersectNormal = -intersectNormal;
		inside = true;
	}

	Ray scattered;
	Vec3f attenuation;
	if (objectMaterial.scatter(ray, intersectPoint, intersectNormal, attenuation, scattered))
	{
		return attenuation * trace(scattered, depth - 1);
	}
		
	return Vec3f(0.0f);

	/*Vec3f target = intersectPoint + intersectNormal + Vec3f::randomDirection();
	Ray ray(intersectPoint, target - intersectPoint);
	return trace(ray, depth + 1) * 0.5f;*/

	//if (objectMaterial.getTransparency() > 0.0f || objectMaterial.getReflectivity() > 0.0f && depth < MAX_RAY_DEPTH)
	//{
	//	float facingRatio = -rayDir.dot(nhit);
	//	// change the mix value to tweak the effect
	//	float fresnel = pow(fminf(1.0f, fmaxf(0.0f, 1.0f - facingRatio)), 5.0f);
	//	// compute reflection direction (not need to normalize because all vectors
	//	// are already normalized)
	//	Vec3f reflDir = rayDir - nhit * 2.0f * rayDir.dot(nhit);
	//	reflDir.normalize();
	//	Vec3f reflection = trace(phit + nhit * bias, reflDir, depth + 1.0f);
	//	
	//	Vec3f refraction = objectMaterial.getBaseColour();
	//	// if the sphere is also transparent compute refraction ray (transmission)
	//	if (objectMaterial.getTransparency())
	//	{
	//		float ior = 1.1f, eta = (inside) ? ior : 1.0f / ior; // are we inside or outside the surface?
	//		float cosi = -nhit.dot(rayDir);
	//		float k = 1.0f - eta * eta * (1.0f - cosi * cosi);
	//		Vec3f refrDir = rayDir * eta + nhit * (eta *  cosi - sqrt(k));
	//		refrDir.normalize();
	//		refraction = trace(phit - nhit * bias, refrDir, depth + 1.0f);
	//	}
	//	// the result is a mix of reflection and refraction (if the sphere is transparent)
	//	surfaceColor = (reflection * fresnel + refraction * (1.0f - fresnel) * objectMaterial.getTransparency()) * objectMaterial.getBaseColour();
	//}
	//else
	//{
	//	// it's a diffuse object, only look for lights
	//	for (unsigned i = 0; i < m_objects.size(); ++i)
	//	{
	//		if (m_objects[i]->isActive())
	//		{
	//			if (m_objects[i]->getMaterial().getEmissiveColour().x > 0.0f)
	//			{
	//				// this is a light
	//				Vec3f transmission = 1;
	//				Vec3f lightDirection = m_objects[i]->getPosition() - phit;
	//				lightDirection.normalize();

	//				for (unsigned j = 0; j < m_objects.size(); ++j)
	//				{
	//					if (i != j)
	//					{
	//						if (m_objects[j]->isActive())
	//						{
	//							float t0, t1;
	//							if (m_objects[j]->intersect(phit + nhit * bias, lightDirection, t0, t1))
	//							{
	//								transmission = 0;
	//								break;
	//							}
	//						}
	//					}
	//				}

	//				surfaceColor += objectMaterial.getBaseColour() * transmission * std::max(float(0), nhit.dot(lightDirection)) * m_objects[i]->getMaterial().getEmissiveColour();
	//			}
	//		}
	//	}
	//}

	//return surfaceColor + objectMaterial.getEmissiveColour();
}

void Raytracer::run()
{
	m_currentFrame = 0;

	m_imageBuffer = new Vec3f[m_width * m_height];

	for (int i = 0; i < m_frameCount; ++i)
	{
		updateAllObjects();

		renderFrame();

		m_currentFrame++;
	}

	delete[] m_imageBuffer;
}


//[comment]
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
//[/comment]
void Raytracer::renderFrame()
{
	//Octree octree = Octree();

	Vec3f* pixel = m_imageBuffer;

	float invWidth = 1.0f / float(m_width), invHeight = 1.0f / float(m_height);
	float fov = 30.0f, aspectratio = m_width / float(m_height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);

	// Trace rays
	for (float y = 0.0f; y < m_height; ++y)
	{
		for (float x = 0.0f; x < m_width; ++x, ++pixel)
		{
			Vec3f pixelColour;

			for (int sampleIndex = 0; sampleIndex < m_samplesPerPixel; ++sampleIndex)
			{
				float u = x + static_functions::randomNegativeOneToOne();
				float v = y + static_functions::randomNegativeOneToOne();

				float xx = (2.0f * ((u + 0.5f) * invWidth) - 1.0f) * angle * aspectratio;
				float yy = (1.0f - 2.0f * ((v + 0.5f) * invHeight)) * angle;
				Vec3f raydir(xx, yy, -1.0f);
				raydir.normalize();

				Ray ray(Vec3f(0.0f), raydir);
				pixelColour += trace(ray, 0);
			}

			float scale = 1.0f / float(m_samplesPerPixel);
			pixelColour.x = sqrt(scale * pixelColour.x);
			pixelColour.y = sqrt(scale * pixelColour.y);
			pixelColour.z = sqrt(scale * pixelColour.z);

			*pixel = pixelColour;
		}
	}

	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::stringstream ss;
	ss << "raytracer/output/spheres" << m_currentFrame << ".ppm";

	std::string tempString = ss.str();
	char* filename = (char*)tempString.c_str();

	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	ofs << "P6\n" << m_width << " " << m_height << "\n255\n";
	for (unsigned i = 0; i < m_width * m_height; ++i)
	{
		ofs << (unsigned char)(std::min(1.0f, m_imageBuffer[i].x) * 255.0f) <<
			(unsigned char)(std::min(1.0f, m_imageBuffer[i].y) * 255.0f) <<
			(unsigned char)(std::min(1.0f, m_imageBuffer[i].z) * 255.0f);
	}
	ofs.close();

	std::cout << "Rendered F" << m_currentFrame << '\n';
}
