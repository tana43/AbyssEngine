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
        //判定処理するか
        bool enabled_ = true;

        std::string filePath_;

        //タグ　同じタグ同士では判定を取らないようにする
        unsigned int tag_;

        //
        int type_;
    };
}
