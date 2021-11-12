#include "Material.h"

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

float Material::getTransparency() const
{
	return m_transparency;
}

float Material::getReflectivity() const
{
	return m_reflectivity;
}
