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
        void Initialize(const std::shared_ptr<Actor>& actor)override; //������
        bool DrawImGui() override;
        //bool CanMultiple() override;
        void Update();

        void SetEnableDebugController(const bool& enable) { enableDebugController_ = enable; }
        void SetIsMainCamera(const bool& isMainCamera) { isMainCamera_ = isMainCamera; }

        const bool& GetIsMainCamera() const { return isMainCamera_; }

        //�J���������Ă���������O�����ɂȂ�悤�Ȑ��K���ς݃x�N�g�����v�Z����
        //��{�R���g���[���[�̓��͒l�ɑ΂��Ă����g��Ȃ�
        Vector3 ConvertTo2DVectorFromCamera(const Vector2& v);//XZ���̂݌v�Z
        Vector3 ConvertTo3DVectorFromCamera(const Vector2& v);//Y�����v�Z

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
        //std::array<Vector4, 6> frustumPlanes_; //������J�����O�pPlane
        Vector3 eye_;
        Vector3 focus_;

        bool isMainCamera_ = false;

        Vector3 socketOffset_;//View�̃I�t�Z�b�g���W
        Vector3 targetOffset_;//Focus�̃I�t�Z�b�g���W
        float armLength_ = 1.0f;//�J��������^�[�Q�b�g�܂ł̋���

        bool enableCameraLag_ = true;//�^�[�Q�b�g�ւ̃J�����̒Ǐ]��x��������
        float cameraLagSpeed_ = 10.0f;//�J�����̒Ǐ]���x
        CONSTANT_FLOAT Camera_Lag_Max_Distance = 3.0f;//�J�������x�����邱�Ƃ��ł���ő勗��

        //�t���X�^���J�����O�p
        DirectX::BoundingFrustum frustum_;

        friend class RenderManager;

    private: 
        float debCameraSpeed_       = 10.0f;//�f�o�b�O���̃J�����X�s�[�h
        bool enableDebugController_ = true; //�f�o�b�O����I���A�I�t

        Transform* viewTarget_;             //�r���[�^�[�Q�b�g

        //�t���X�^���̏������
        DirectX::BoundingFrustum initialFrustum_;
    };
}


