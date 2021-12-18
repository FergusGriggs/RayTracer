#include "Material.h"

Material::Material() :
	m_baseColour(Vec3f(0.0f)),
	m_emissiveColour(Vec3f(0.0f)),

	m_transparency(0.0f),
	m_reflectivity(0.0f)
{
}

Material::Material(const Vec3f& baseColour, float transparency, float reflectivity, const Vec3f& emissiveColour) :
	m_baseColour(baseColour),
	m_emissiveColour(emissiveColour),

	m_transparency(transparency),
	m_reflectivity(reflectivity)
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
