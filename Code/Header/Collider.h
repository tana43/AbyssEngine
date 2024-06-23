#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <string>

namespace AbyssEngine
{
    class Collider : public AbyssEngine::Component
    {
    public:
        Collider() {}
        ~Collider() {}

        //void Initialize(const std::shared_ptr<Actor>& actor);

        void SetFilePath(const char* path) { filePath_ = path; }

        void SetEnable(bool value) { enabled_ = value; }
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    public:
        struct Triangle
        {
            DirectX::XMFLOAT3	positions[3];
            DirectX::XMFLOAT3	normal;
        };

    protected:
        bool enabled_ = true;//îªíËèàóùÇ∑ÇÈÇ©

        std::string filePath_;

    };
}
