#pragma once
#include "Component.h"
#include "MathHelper.h"
#include <string>

#include "CollisionHelper.h"

namespace AbyssEngine
{
    class Collider : public AbyssEngine::Component
    {
    public:
        enum class Tag : unsigned int
        {
            Non     = 0x00,
            Player  = 0x01 << 0,
            Enemy   = 0x01 << 1,
        };

        enum class Type
        {
            Mesh,
            Sphere,
            AABB
        };

    public:
        Collider() {}
        ~Collider() {}

        void Initialize(const std::shared_ptr<Actor>& actor);

        void SetFilePath(const char* path) { filePath_ = path; }

        [[nodiscard]] bool GetEnabled() const { return enabled_; }
        void SetEnable(bool value) { enabled_ = value; }

        const unsigned int& GetTag() const { return tag_; }
        void ReplaceTag(unsigned int t) { tag_ = t; }
        void ReplaceTag(Tag t) { tag_ = static_cast<unsigned int>(t); }
        void AddTag(Tag t)
        {
            tag_ |= static_cast<unsigned int>(t);
        }

    public:
        struct Triangle
        {
            DirectX::XMFLOAT3	positions[3];
            DirectX::XMFLOAT3	normal;
        };

    protected:
        //���菈�����邩
        bool enabled_ = true;

        std::string filePath_;

        //�^�O�@�����^�O���m�ł͔�������Ȃ��悤�ɂ���
        unsigned int tag_;

        //
        int type_;
    };
}
