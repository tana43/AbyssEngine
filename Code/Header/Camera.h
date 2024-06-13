#pragma once
#include <memory>
#include "Component.h"
#include "MathHelper.h"
#include "SystemFunction.h"
#include <DirectXCollision.h>

namespace AbyssEngine
{
    class RenderManager;

    class Camera : public Component
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor)override; //初期化
        bool DrawImGui() override;
        //bool CanMultiple() override;
        void Update();

        void SetEnableDebugController(const bool& enable) { enableDebugController_ = enable; }
        void SetIsMainCamera(const bool& isMainCamera) { isMainCamera_ = isMainCamera; }

        const bool& GetIsMainCamera() const { return isMainCamera_; }

        //カメラが見ている方向が前方向になるような正規化済みベクトルを計算する
        //基本コントローラーの入力値に対してしか使わない
        Vector3 ConvertTo2DVectorFromCamera(const Vector2& v);//XZ軸のみ計算
        Vector3 ConvertTo3DVectorFromCamera(const Vector2& v);//Y軸も計算

        void UpdateFrustum();

    public:
        void SetViewTarget(Transform* t) { viewTarget_ = t; }
        [[nodiscared]] Transform* GetViewTarget() { return viewTarget_; }

    private:
        void DebugCameraController();

        void CameraLagUpdate();

    public:
        float fov_ = DirectX::XMConvertToRadians(60.0f);
        float nearZ_ = 0.1f;
        float farZ_ = 1000.0f;
        float orthographicSize_ = 1000.0f;
        bool isOrthographic = false;
        Matrix viewMatrix_ = {};
        Matrix projectionMatrix_ = {};
        Matrix viewProjectionMatrix_ = {};
        //std::array<Vector4, 6> frustumPlanes_; //視錐台カリング用Plane
        Vector3 eye_;
        Vector3 focus_;

        bool isMainCamera_ = false;

        Vector3 socketOffset_;//Viewのオフセット座標
        Vector3 targetOffset_;//Focusのオフセット座標
        float armLength_ = 1.0f;//カメラからターゲットまでの距離

        bool enableCameraLag_ = true;//ターゲットへのカメラの追従を遅延させる
        float cameraLagSpeed_ = 10.0f;//カメラの追従速度
        CONSTANT_FLOAT Camera_Lag_Max_Distance = 3.0f;//カメラが遅延することができる最大距離

        //フラスタムカリング用
        DirectX::BoundingFrustum frustum_;

        friend class RenderManager;

    private: 
        float debCameraSpeed_       = 10.0f;//デバッグ中のカメラスピード
        bool enableDebugController_ = true; //デバッグ操作オン、オフ

        Transform* viewTarget_;             //ビューターゲット

        //フラスタムの初期状態
        DirectX::BoundingFrustum initialFrustum_;
    };
}


