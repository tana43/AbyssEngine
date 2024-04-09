#pragma once
#include <memory>
#include "Component.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class Camera : public Component
    {
    public:
        float fov_ = 60.0f;
        float nearZ_ = 0.1f;
        float farZ_ = 1000.0f;
        float orthographicSize_ = 1000.0f;
        bool isOrthographic = false;
        Matrix viewMatrix_ = {};
        Matrix projectionMatrix_ = {};
        Matrix viewProjectionMatrix_ = {};
        //std::array<Vector4, 6> frustumPlanes_; //‹‘äƒJƒŠƒ“ƒO—pPlane

    };
}


