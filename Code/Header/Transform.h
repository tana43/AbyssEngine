#pragma once
#include "MathHelper.h"
#include "Actor.h"

namespace AbyssEngine
{
    class Transform final : public Component
    {
    public:
        Transform() {}
        Transform(Vector3 position, Vector4 quaternion) {}
        Transform(Vector3 position, Vector3 eular) {}

        Matrix CalcWorldMatrix();

        //getter setter
        [[nodiscard]] const Vector3& GetPosition() const { return position_; };
        void SetPosition(const Vector3& position) { position_ = position; }
        void SetPositionX(const float& x) { position_.x = x; }
        void SetPositionY(const float& y) { position_.y = y; }
        void SetPositionZ(const float& z) { position_.z = z; }

        [[nodiscard]] const Vector4& GetRotation() const { return rotation_; };
        void SetRotation(const Vector4& rotation) { rotation_ = rotation; }
        void SetRotationX(const float& x) { rotation_.x = x; }
        void SetRotationY(const float& y) { rotation_.y = y; }
        void SetRotationZ(const float& z) { rotation_.z = z; }

        [[nodiscard]] const Vector3& GetScale() const { return scale_; };
        [[nodiscard]] const float& GetScaleFactor() const { return scaleFactor_; };
        void SetScale(const Vector3& scale) { scale_ = scale; }
        void SetScaleX(const float& x) { scale_.x = x; }
        void SetScaleY(const float& y) { scale_.y = y; }
        void SetScaleZ(const float& z) { scale_.z = z; }
        void SetScaleFactor(const float& scale) { scaleFactor_ = scale; }

        [[nodiscard]] const Vector3& GetForward() const { return forward_; }
        [[nodiscard]] const Vector3& GetRight() const { return right_; }
        [[nodiscard]] const Vector3& GetUp() const { return up_; }

        Vector3 GetEulerAngles() const;

        bool DrawImGui()override;

    private:
        Vector3 position_ = { 0,0,0 };
        Quaternion rotation_ = { 0,0,0,1 };
        Vector3 scale_ = { 1,1,1 };
        float scaleFactor_ = 1.0f;

        Vector3 forward_ = { 0.0f,0.0f,1.0f };
        Vector3 right_ = { 1.0f,0.0f,0.0f };
        Vector3 up_ = { 0.0f,1.0f,0.0f };
    };
}

