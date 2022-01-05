#pragma once

#include "vec3.h"
#include "ray.h"

class Camera {
public:
    Camera(Vec3f position, Vec3f lookAt, float verticalFov, float aspectRatio, float aperture, float focusDist);
    ~Camera();

    Ray getRay(float u, float v) const;

private:
    Vec3f m_position;
    Vec3f m_bottomLeft;

    Vec3f m_horizontal;
    Vec3f m_vertical;

    Vec3f m_front;
    Vec3f m_right;
    Vec3f m_up;

    float m_lensRadius;
};