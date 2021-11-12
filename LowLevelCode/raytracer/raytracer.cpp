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

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

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

					float reflectivity = (*materialItr)["reflectivity"].GetFloat();
					float transparency = (*materialItr)["transparency"].GetFloat();

					Material material(baseColour, transparency, reflectivity, emissiveColour);
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

					KeyFramedValue<Vec3f> positions;
					KeyFramedValue<float> radii;

					if (objectItr->HasMember("position_keyframes"))
					{
						rapidjson::Value& positionKeyframes = (*objectItr)["position_keyframes"];

						if (positionKeyframes.IsArray())
						{
							for (auto positionKeyframeItr = positionKeyframes.Begin(); positionKeyframeItr != positionKeyframes.End(); ++positionKeyframeItr)
							{
								Vec3f position = getFloat3FromArray((*positionKeyframeItr)["value"].GetArray());
								float time = (*positionKeyframeItr)["time"].GetFloat();

								positions.addKeyFrame(KeyFrame<Vec3f>(position, time));
							}
						}
					}

					if (objectItr->HasMember("radius_keyframes"))
					{
						rapidjson::Value& radiusKeyframes = (*objectItr)["radius_keyframes"];

						if (radiusKeyframes.IsArray())
						{
							for (auto radiusKeyframeItr = radiusKeyframes.Begin(); radiusKeyframeItr != radiusKeyframes.End(); ++radiusKeyframeItr)
							{
								float radius = (*radiusKeyframeItr)["value"].GetFloat();
								float time = (*radiusKeyframeItr)["time"].GetFloat();

								radii.addKeyFrame(KeyFrame<float>(radius, time));
							}
						}
					}

					Sphere* sphere = new Sphere(positions, m_materials[materialId], radii);
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
Vec3f Raytracer::trace(const Vec3f &rayOrigin, const Vec3f &rayDir, int depth) const
{
	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	float tnear = INFINITY;
	const Object* object = nullptr;
	// find intersection of this ray with the sphere in the scene
	for (unsigned i = 0; i < m_objects.size(); ++i)
	{
		float t0 = INFINITY, t1 = INFINITY;
		if (m_objects[i]->intersect(rayOrigin, rayDir, t0, t1))
		{
			if (t0 < 0) t0 = t1;
			if (t0 < tnear)
			{
				tnear = t0;
				object = m_objects[i];
			}
		}
	}
	// if there's no intersection return white
	if (object == nullptr)
	{
		return Vec3f(1.0f);
	}

	const Material& objectMaterial = object->getMaterial();

	Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
	Vec3f phit = rayOrigin + rayDir * tnear; // point of intersection
	Vec3f nhit = phit - object->getPosition(); // normal at the intersection point
	nhit.normalize(); // normalize normal direction
					  // If the normal and the view direction are not opposite to each other
					  // reverse the normal direction. That also means we are inside the sphere so set
					  // the inside bool to true. Finally reverse the sign of IdotN which we want
					  // positive.
	float bias = 1e-4; // add some bias to the point from which we will be tracing
	bool inside = false;

	if (rayDir.dot(nhit) > 0.0f)
	{
		nhit = -nhit; 
		inside = true;
	}

	if ((objectMaterial.getTransparency() > 0.0f || objectMaterial.getReflectivity() > 0.0f) && depth < MAX_RAY_DEPTH)
	{
		float facingratio = -rayDir.dot(nhit);
		// change the mix value to tweak the effect
		float fresneleffect = mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.1f);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Vec3f reflDir = rayDir - nhit * 2 * rayDir.dot(nhit);
		reflDir.normalize();
		Vec3f reflection = trace(phit + nhit * bias, reflDir, depth + 1);
		Vec3f refraction = 0;
		// if the sphere is also transparent compute refraction ray (transmission)

		if (objectMaterial.getTransparency())
		{
			float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
			float cosi = -nhit.dot(rayDir);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3f refrDir = rayDir * eta + nhit * (eta *  cosi - sqrt(k));
			refrDir.normalize();
			refraction = trace(phit - nhit * bias, refrDir, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * objectMaterial.getTransparency()) * objectMaterial.getBaseColour();
	}
	else
	{
		// it's a diffuse object, no need to raytrace any further
		for (unsigned i = 0; i < m_objects.size(); ++i)
		{
			if (m_objects[i]->getMaterial().getEmissiveColour().x > 0)
			{
				// this is a light
				Vec3f transmission = 1;
				Vec3f lightDirection = m_objects[i]->getPosition() - phit;
				lightDirection.normalize();

				for (unsigned j = 0; j < m_objects.size(); ++j)
				{
					if (i != j)
					{
						float t0, t1;
						if (m_objects[j]->intersect(phit + nhit * bias, lightDirection, t0, t1))
						{
							transmission = 0;
							break;
						}
					}
				}

				surfaceColor += objectMaterial.getBaseColour() * transmission * std::max(float(0), nhit.dot(lightDirection)) * m_objects[i]->getMaterial().getEmissiveColour();
			}
		}
	}

	return surfaceColor + objectMaterial.getEmissiveColour();
}

void Raytracer::run()
{
	m_currentFrame = 0;

	for (int i = 0; i < 100; ++i)
	{
		updateAllObjects();

		renderFrame();

		m_currentFrame++;
	}
}


//[comment]
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
//[/comment]
void Raytracer::renderFrame()
{
	Vec3f* image = new Vec3f[m_width * m_height];
	Vec3f* pixel = image;

	float invWidth = 1 / float(m_width), invHeight = 1 / float(m_height);
	float fov = 30.0f, aspectratio = m_width / float(m_height);
	float angle = tan(M_PI * 0.5 * fov / 180.);

	// Trace rays
	for (unsigned y = 0; y < m_height; ++y)
	{
		for (unsigned x = 0; x < m_width; ++x, ++pixel)
		{
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3f raydir(xx, yy, -1);
			raydir.normalize();
			*pixel = trace(Vec3f(0.0f), raydir, 0);
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
		ofs << (unsigned char)(std::min(1.0f, image[i].x) * 255) <<
			(unsigned char)(std::min(1.0f, image[i].y) * 255) <<
			(unsigned char)(std::min(1.0f, image[i].z) * 255);
	}
	ofs.close();
	delete[] image;
}

void Raytracer::basicRender()
{
	//removeAllObjects();
	//m_currentFrame = 0;

	//Material groundMaterial = Material(Vec3f(0.2f, 0.2f, 0.2f));

	//Material sphereMaterial1 = Material(Vec3f(1.0f, 0.32f, 0.36f), 1.0f, 0.5f);
	//Material sphereMaterial2 = Material(Vec3f(0.9f, 0.76f, 0.46f), 1.0f, 0.0f);
	//Material sphereMaterial3 = Material(Vec3f(0.65f, 0.77f, 0.97f), 1.0f, 0.0f);

	//m_objects.push_back(new Sphere(Vec3f(0.0f, -10004.0f, -20.0f), groundMaterial, 10000.0f));
	//m_objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, 4.0f));
	//m_objects.push_back(new Sphere(Vec3f(5.0f, -1.0f, -15.0f), sphereMaterial2, 2.0f));
	//m_objects.push_back(new Sphere(Vec3f(5.0f, 0.0f, -25.0f), sphereMaterial3, 3.0f));
	//
	//// This creates a file, titled 1.ppm in the current working directory
	//renderFrame();

}

void Raytracer::simpleShrinking()
{
	//std::vector<Object*> objects;

	//Material groundMaterial = Material(Vec3f(0.2f, 0.2f, 0.2f));

	//Material sphereMaterial1 = Material(Vec3f(1.0f, 0.32f, 0.36f), 1.0f, 0.5f);
	//Material sphereMaterial2 = Material(Vec3f(0.9f, 0.76f, 0.46f), 1.0f, 0.0f);
	//Material sphereMaterial3 = Material(Vec3f(0.65f, 0.77f, 0.97f), 1.0f, 0.0f);

	//for (int i = 0; i < 4; i++)
	//{
	//	objects.push_back(new Sphere(Vec3f(0.0f, -10004.0f, -20.0f), groundMaterial, 10000.0f));

	//	if (i == 0)
	//	{
	//		objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, 4.0f)); // The radius paramter is the value we will change
	//	}
	//	else if (i == 1)
	//	{
	//		objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, 3.0f)); // Radius--
	//	}
	//	else if (i == 2)
	//	{
	//		objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, 2.0f)); // Radius--
	//	}
	//	else if (i == 3)
	//	{
	//		objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, 1.0f)); // Radius--
	//	}

	//	objects.push_back(new Sphere(Vec3f(5.0f, -1.0f, -15.0f), sphereMaterial2, 2.0f));
	//	objects.push_back(new Sphere(Vec3f(5.0f, 0.0f, -25.0f), sphereMaterial3, 3.0f));

	//	renderFrame();

	//	removeAllObjects();
	//}
}

void Raytracer::smoothScaling()
{
	//m_currentFrame = 0;
	//Material groundMaterial = Material(Vec3f(0.2f, 0.2f, 0.2f));

	//Material sphereMaterial1 = Material(Vec3f(1.0f, 0.32f, 0.36f), 1.0f, 0.5f);
	//Material sphereMaterial2 = Material(Vec3f(0.9f, 0.76f, 0.46f), 1.0f, 0.0f);
	//Material sphereMaterial3 = Material(Vec3f(0.65f, 0.77f, 0.97f), 1.0f, 0.0f);

	//for (int frame = 0; frame <= 100; frame++)
	//{
	//	m_objects.push_back(new Sphere(Vec3f(0.0f, -10004.0f, -20.0f), groundMaterial, 10000.0f));
	//	m_objects.push_back(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), sphereMaterial1, static_cast<float>(frame) / 100.0f)); // Radius++ change here
	//	m_objects.push_back(new Sphere(Vec3f(5.0f, -1.0f, -15.0f), sphereMaterial2, 2.0f));
	//	m_objects.push_back(new Sphere(Vec3f(5.0f, 0.0f, -25.0f), sphereMaterial3, 3.0f));

	//	renderFrame();

	//	std::cout << "Rendered and saved spheres" << frame << ".ppm" << std::endl;

	//	removeAllObjects();
	//	m_currentFrame++;
	//}
}
