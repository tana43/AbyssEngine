#pragma once
#include <memory>
#include "Component.h"
#include "MathHelper.h"
#include "SystemFunction.h"
#include "PerlinNoise.h"
#include "FadeSystem.h"

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

        //メインのカメラを変更する
        static void ChangeMainCamera(Camera* c);

        //カメラをズームイン・アウトさせる
        void Zoom();
    public:
        void SetViewTarget(Transform* t) { viewTarget_ = t; }
        Transform* GetViewTarget() { return viewTarget_; }

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
        DirectX::BoundingFrustum shadowInitialFrustum_;

        Vector2 mouseSensitivity_ = { 2.768f ,2.768f };//カメラ感度

        //カメラの寄り引き
        std::unique_ptr<FadeSystem> zoomParams_;

        //カメラシェイク
    public:
        struct  CameraShake
        {
            struct Position//位置変更による揺れのパラメータ
            {
                float positionAmplitudeMultiplier_ = 1.0f;//すべての位置の揺れに対する振幅の乗数
                float positionFrequencyMultiplier_ = 1.0f;//すべての位置の揺れに対する周波数の乗数
                DirectX::XMFLOAT3 positionAmplitude_ = { 1.0f,1.0f,1.0f };//各軸の振幅
                DirectX::XMFLOAT3 positionFrequency_ = { 1.0f,1.0f,1.0f };//各軸の周波数
            }position_;

            struct Rotation
            {
                float rotationAmplitudeMultiplier_ = 1.0f;//すべての位置の揺れに対する振幅の乗数
                float rotationFrequencyMultiplier_ = 1.0f;//すべての位置の揺れに対する周波数の乗数
                DirectX::XMFLOAT3 rotationAmplitude_ = { 1.0f,1.0f,1.0f };//各軸の振幅 
                DirectX::XMFLOAT3 rotationFrequency_ = { 1.0f,1.0f,1.0f };//各軸の周波数 
            }rotation_;

            struct Timing//揺れの持続時間等
            {
                float duration_ = 1.0f;//揺れの持続時間
                float blendInTime_ = 0;//この揺れのフェードインの時間
                float blendOutTime_ = 0;//この揺れのフェードアウトの時間
            }timing_;
        };
        struct  CameraShakeParameters
        {

            struct Position//位置変更による揺れのパラメータ
            {
                float amplitudeMultiplier_ = 1.0f;//すべての位置の揺れに対する振幅の乗数
                float frequencyMultiplier_ = 1.0f;//すべての位置の揺れに対する周波数の乗数
                Vector3 amplitude_ = { 1.0f,1.0f,1.0f };//各軸の振幅
                Vector3 frequency_ = { 1.0f,1.0f,1.0f };//各軸の周波数
            }position_;

            struct Rotation
            {
                float amplitudeMultiplier_ = 1.0f;//すべての位置の揺れに対する振幅の乗数
                float frequencyMultiplier_ = 1.0f;//すべての位置の揺れに対する周波数の乗数
                Vector3 amplitude_ = { 1.0f,1.0f,1.0f };//各軸の振幅 ※現在Z軸の回転は反映されない
                Vector3 frequency_ = { 1.0f,1.0f,1.0f };//各軸の周波数 ※現在Z軸の回転は反映されない
            }rotation_;

            struct Timing//揺れの持続時間等
            {
                float duration_ = 1.0f;//揺れの持続時間
                float blendInTime_ = 0.1f;//この揺れのフェードインの時間
                float blendOutTime_ = 0.1f;//この揺れのフェードアウトの時間
            }timing_;

            CameraShakeParameters operator=(const CameraShakeParameters& param);
        };

        //カメラ振動　今は重複はできず１つの振動をするだけ
        void CameraShake(CameraShakeParameters shakePram);
        void CameraShakeUpdate();

    private:
        bool activeCameraShake_ = false;

        Vector3 shakePosition_{ 0,0,0 };
        Vector4 shakeRotation_{ 0,0,0,1 };
        float shakeTimer_;
        Vector3 shakePosFreqTimer_;//座標振動の周波数の速度を表すための数値
        Vector3 shakeRotFreqTimer_;//回転振動の周波数の速度を表すための数値

        CameraShakeParameters shakeParam_;
        PerlinNoise shakePerlinNoise_;//カメラ振動用のノイズ
    };
}


