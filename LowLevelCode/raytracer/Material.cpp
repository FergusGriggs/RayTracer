#include "material.h"

Material::Material() :
	m_baseColour(Vec3f(0.0f)),
	m_emissiveColour(Vec3f(0.0f)),

	m_roughness(0.7f),
	m_metallic(0.0f),
	m_transmission(0.0f),
	m_IOR(0.0f)
{
}

Material::Material(const Vec3f& baseColour, float roughness, float metallic, float transmission, float IOR, const Vec3f& emissiveColour) :
	m_baseColour(baseColour),
	m_emissiveColour(emissiveColour),

	m_roughness(roughness),
	m_metallic(metallic),

	m_transmission(transmission),
	m_IOR(IOR)
{
}

Material::~Material()
{
}

const Vec3f& Material::getBaseColour() const
{
	return m_baseColour;
}

const Vec3f& Material::getEmissiveColour() const
{
	return m_emissiveColour;
}

float Material::getTransmission() const
{
	return m_transmission;
}

float Material::getIOR() const
{
	return m_IOR;
}

float Material::getRoughness() const
{
	return m_roughness;
}

float Material::getMetallic() const
{
	return m_metallic;
}

bool Material::scatter(const Ray& in, const TraceResult& traceResult, Vec3f& attenuation, Ray& scattered) const
{
	// Move the intersection away from the surface slightly to avoid self collision
	Vec3f bias = traceResult.m_hitNormal * 1e-3f;
	if (m_metallic > 0.0f)
	{
		Vec3f reflected = Vec3f::reflect(in.m_direction, traceResult.m_hitNormal);
		
		scattered = Ray(traceResult.m_hitPoint + bias, reflected + Vec3f::randomInUnitSphere() * m_roughness);
		attenuation = m_baseColour;
		return Vec3f::dot(scattered.m_direction, traceResult.m_hitNormal) > 0.0f;
	}
	else if (m_transmission > 0.0f)
	{
		attenuation = Vec3f(1.0f);
		float refractionRatio = traceResult.m_inside ? m_IOR : (1.0f / m_IOR);

		Vec3f unitDirection = Vec3f::normalise(in.m_direction);
		Vec3f refracted = Vec3f::refract(unitDirection, traceResult.m_hitNormal, refractionRatio);

		scattered = Ray(traceResult.m_hitPoint - bias, refracted);
		return true;
	}
	else
	{
		Vec3f target = traceResult.m_hitPoint + traceResult.m_hitNormal + Vec3f::randomDirection();
		scattered = Ray(traceResult.m_hitPoint + bias, target - traceResult.m_hitPoint);
		attenuation = m_baseColour;
		return Vec3f::dot(scattered.m_direction, traceResult.m_hitNormal) > 0.0f;
	}

	return false;
}
