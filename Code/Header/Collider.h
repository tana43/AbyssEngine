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

        //�����ɓ������Ă���ꍇ�����ŌĂ΂��
        virtual void OnCollision(const std::shared_ptr<Collider>& collision) = 0;

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
