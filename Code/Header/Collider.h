#pragma once
#include "Component.h"
#include "MathHelper.h"
#include <string>

namespace AbyssEngine
{
    class Collider : public AbyssEngine::Component
    {
    public:
        enum class Tag : unsigned int
        {
            Player  = 0x01 << 1,
            Enemy   = 0x01 << 2,
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

        //void Initialize(const std::shared_ptr<Actor>& actor);

        void SetFilePath(const char* path) { filePath_ = path; }

        [[nodiscard]] bool GetEnabled() const { return enabled_; }
        void SetEnable(bool value) { enabled_ = value; }

        const unsigned int& GetTag() const { return tag_; }
        void SetTag(const Tag& tag) { tag_ = static_cast<unsigned int>(tag); }

        //何かに当たっている場合自動で呼ばれる
        virtual void OnCollision(const std::shared_ptr<Collider>& collision) = 0;

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
