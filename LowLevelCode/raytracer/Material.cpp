#include "Material.h"

Material::Material() :
	m_baseColour(Vec3f(0.0f)),
	m_emissiveColour(Vec3f(0.0f)),

	m_roughness(0.7f),
	m_metallic(0.0f),
	m_transmission(0.0f)
{
}

Material::Material(const Vec3f& baseColour, float roughness, float metallic, float transmission, const Vec3f& emissiveColour) :
	m_baseColour(baseColour),
	m_emissiveColour(emissiveColour),

	m_roughness(roughness),
	m_metallic(metallic),
	m_transmission(transmission)
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

float Material::getRoughness() const
{
	return m_roughness;
}

float Material::getMetallic() const
{
	return m_metallic;
}

bool Material::scatter(const Ray& in, const Vec3f& hitPosition, const Vec3f& hitNormal, Vec3f& attenuation, Ray& scattered) const
{
	if (m_metallic > 0.0f)
	{
		Vec3f reflected = Vec3f::reflect(in.m_direction, hitNormal);
		scattered = Ray(hitPosition, reflected);
		attenuation = m_baseColour;
		return Vec3f::dot(scattered.m_direction, hitNormal) > 0.0f;
	}
	else if (m_transmission > 0.0f)
	{

	}
	else
	{
		Vec3f target = hitPosition + hitNormal + Vec3f::randomDirection();
		scattered = Ray(hitPosition, target - hitPosition);
		attenuation = m_baseColour;
		return Vec3f::dot(scattered.m_direction, hitNormal) > 0.0f;
	}

	return false;
}
