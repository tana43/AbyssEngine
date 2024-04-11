#pragma once
#include <memory>
#include "Component.h"
#include "MathHelper.h"

namespace AbyssEngine
{
    class RenderManager;

    class Camera : public Component
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override; //‰Šú‰»
        bool DrawImGui() override;
        //bool CanMultiple() override;
        void Update();

    public:
        float fov_ = DirectX::XMConvertToRadians(60.0f);
        float nearZ_ = 0.1f;
        float farZ_ = 1000.0f;
        float orthographicSize_ = 1000.0f;
        bool isOrthographic = false;
        Matrix viewMatrix_ = {};
        Matrix projectionMatrix_ = {};
        Matrix viewProjectionMatrix_ = {};
        //std::array<Vector4, 6> frustumPlanes_; //‹‘äƒJƒŠƒ“ƒO—pPlane

        friend class RenderManager;

    };
}


