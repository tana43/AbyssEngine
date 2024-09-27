#pragma once
#include "MathHelper.h"
#include "Actor.h"

namespace AbyssEngine
{
    class Transform final : public Component
    {
    public:
        Transform() {}
        Transform(Vector3 position_, Vector4 quaternion) {}
        Transform(Vector3 position_, Vector3 eular) {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        Matrix CalcWorldMatrix();//ワールド行列更新
        Matrix CalcLocalMatrix();//ローカル行列更新

        [[nodiscard]] const Matrix& GetWorldMatrix() const { return worldMatrix_; }
        void SetWorldMatrix(const Matrix& mat);

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
        void SetScaleFactor(const float& scale_) { scaleFactor_ = scale_; }

        //getter setter
        [[nodiscard]] const Vector3& GetLocalPosition() const { return localPosition_; }
        void SetLocalPosition(const Vector3& position_) { localPosition_ = position_; }
        void SetLocalPositionX(const float& x) { localPosition_.x = x; }
        void SetLocalPositionY(const float& y) { localPosition_.y = y; }
        void SetLocalPositionZ(const float& z) { localPosition_.z = z; }

        [[nodiscard]] const Vector4& GetLocalRotation() const { return localRotation_; }
        void SetLocalRotation(const Vector4& rotation) { localRotation_ = rotation; }
        void SetLocalRotationX(const float& x) { localRotation_.x = x; }
        void SetLocalRotationY(const float& y) { localRotation_.y = y; }
        void SetLocalRotationZ(const float& z) { localRotation_.z = z; }

        [[nodiscard]] const Vector3& GetLocalScale() const { return localScale_; };
        [[nodiscard]] const float& GetLocalScaleFactor() const { return localScaleFactor_; }
        void SetLocalScale(const Vector3& scale) { localScale_ = scale; }
        void SetLocalScaleX(const float& x) { localScale_.x = x; }
        void SetLocalScaleY(const float& y) { localScale_.y = y; }
        void SetLocalScaleZ(const float& z) { localScale_.z = z; }
        void SetLocalScaleFactor(const float& scale_) { localScaleFactor_ = scale_; }

        [[nodiscard]] const Vector3& GetForward() const { return forward_; }
        [[nodiscard]] const Vector3& GetRight() const { return right_; }
        [[nodiscard]] const Vector3& GetUp() const { return up_; }

        Vector3 GetEulerAngles() const;

        void DrawImGui()override;

        //現在の位置、回転、スケールをJsonファイルに保存
        void SaveToJson();

    private:
        Vector3 position_ = { 0,0,0 };
        Quaternion rotation_ = { 0,0,0,1 };
        Vector3 scale_ = { 1,1,1 };
        float scaleFactor_ = 1.0f;

        Vector3 localPosition_ = { 0,0,0 };
        Quaternion localRotation_ = { 0,0,0,1 };
        Vector3 localScale_ = { 1,1,1 };
        float localScaleFactor_ = 1.0f;

        Vector3 forward_ = { 0.0f,0.0f,1.0f };
        Vector3 right_ = { 1.0f,0.0f,0.0f };
        Vector3 up_ = { 0.0f,1.0f,0.0f };

        Matrix worldMatrix_;
        Matrix localMatrix_;

#if _DEBUG
        bool doubleCheckFlag_ = false;
#endif // _DEBUG

    };
}

