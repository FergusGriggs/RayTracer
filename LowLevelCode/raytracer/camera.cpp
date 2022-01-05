#include "camera.h"

#include "math_defines.h"

Camera::Camera(Vec3f position, Vec3f lookAt, float verticalFov, float aspectRatio, float aperture, float focusDist) :
    m_position(position),
    m_lensRadius(aperture / 2.0f)
{
    float theta = verticalFov * M_PI / 180.0f;
    float h = tan(theta / 2.0f);

    float viewportHeight = 2.0f * h;
    float viewportWidth = aspectRatio * viewportHeight;

    m_front = Vec3f::normalise(lookAt - position);
    m_right = Vec3f::normalise(Vec3f::cross(m_front, Vec3f(0.0f, 1.0f, 0.0f)));
    m_up = Vec3f::cross(m_right, m_front);

    if (focusDist < 0.0f)
    {
        focusDist = (lookAt - position).length();
    }

    m_horizontal = -m_right * focusDist * viewportWidth;
    m_vertical = -m_up * focusDist * viewportHeight;

    m_bottomLeft = m_position - (m_horizontal / 2.0f) - (m_vertical / 2.0f) + m_front * focusDist;
}

Camera::~Camera()
{
}

Ray Camera::getRay(float u, float v) const
{
    Vec3f randomInDisc = Vec3f::randomInUnitDisk() * m_lensRadius;
    Vec3f depthOfFieldOffset = -m_right * randomInDisc.x + -m_up * randomInDisc.y;

    return Ray(m_position + depthOfFieldOffset, Vec3f::normalise(m_bottomLeft + (m_horizontal * u) + (m_vertical * v)  - m_position - depthOfFieldOffset));
}
