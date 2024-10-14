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
        void Initialize(const std::shared_ptr<Actor>& actor)override; //������
        void DrawImGui() override;
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

        //���C���̃J������ύX����
        static void ChangeMainCamera(Camera* c);

        static const std::shared_ptr<Camera>& GetMainCamera();

        //�Y�[���ɂ���ĕύX�\�ȃp�����[�^
        struct ZoomParam
        {
            float armLength_ = 1.0f;
            Vector3 socketOffset_ = {0.0f,0.0f,0.0f};
            Vector3 targetOffset_ = {0.0f,0.0f,0.0f};
            float time_ = 0.0f;
        };

        //�J�������Y�[���C���E�A�E�g������
        void Zoom(const ZoomParam& param);

        //�Y�[�������ɖ߂�
        void ZoomReset(const float time);

        Vector2 WorldToScreenPosition(Vector3 worldPosition); //���[���h���W����X�N���[�����W��
        Vector3 WorldToViewportPosition(Vector3 worldPosition); //���[���h���W����r���[�|�[�g���W��

    private:
        void ZoomUpdate();

        //�Y�[���֐����Ă΂ꂽ�ۂ̌��݂̃J��������ێ�����
        ZoomParam retainZoomParam_;

        //�Y�[��������̃J��������ێ�����
        ZoomParam nextZoomParam_;

        //�Y�[������ۂɎg�p����^�C�}�[
        float zoomTimer_;

        //�Y�[�������H
        bool isZooming_ = false;

    public:
        void SetViewTarget(Transform* t) { viewTarget_ = t; }
        Transform* GetViewTarget() { return viewTarget_; }

    private:
        void DebugCameraController();

        void CameraLagUpdate();

    public:
        const Matrix& GetViewMatrix() const { return viewMatrix_; }
        const Matrix& GetProjectionMatrix() const { return projectionMatrix_; }
        const Matrix& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

        const Vector3& GetEye() const { return eye_; }
        const Vector3& GetFocus() const { return focus_; }

        void SetBaseSocketOffset(const Vector3& offset) { baseSocketOffset_ = offset; }
        void SetBaseTargetOffset(const Vector3& offset) { baseTargetOffset_ = offset; }
        void SetBaseArmLength(const float& length) { baseArmLength_ = length; }

        const Vector3& GetSocketOffset() const { return socketOffset_; }
        const Vector3& GetTargetOffset() const { return targetOffset_; }
        const float& GetArmLength() const { return armLength_; }
        const float& GetCameraLagSpeed() const { return cameraLagSpeed_; }
        void SetSocketOffset(const Vector3& offset) { socketOffset_ = offset; }
        void SetTargetOffset(const Vector3& offset) { targetOffset_ = offset; }
        void SetArmLength(const float& length) { armLength_ = length; }

        void SetCameraLagSpeed(const float& speed) { cameraLagSpeed_ = speed; }

        void SetFov(const float& fov) { fov_ = fov; }

    private: 
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

        Vector3 baseSocketOffset_;//��ƂȂ�View�̃I�t�Z�b�g���W
        Vector3 baseTargetOffset_;//��ƂȂ�Focus�̃I�t�Z�b�g���W
        float baseArmLength_ = 1.0f;//��ƂȂ�J��������^�[�Q�b�g�܂ł̋���

        Vector3 socketOffset_;//���ۂɎg�p����View�̃I�t�Z�b�g���W
        Vector3 targetOffset_;//���ۂɎg�p����Focus�̃I�t�Z�b�g���W
        float armLength_ = 1.0f;//���ۂɎg�p����J��������^�[�Q�b�g�܂ł̋���

        bool enableCameraLag_ = true;//�^�[�Q�b�g�ւ̃J�����̒Ǐ]��x��������
        float cameraLagSpeed_ = 10.0f;//�J�����̒Ǐ]���x
        CONSTANT_FLOAT Camera_Lag_Max_Distance = 3.0f;//�J�������x�����邱�Ƃ��ł���ő勗��

        //�t���X�^���J�����O�p
        DirectX::BoundingFrustum frustum_;

        friend class RenderManager;

        float debCameraSpeed_       = 10.0f;//�f�o�b�O���̃J�����X�s�[�h
        bool enableDebugController_ = true; //�f�o�b�O����I���A�I�t

        Transform* viewTarget_;             //�r���[�^�[�Q�b�g

        //�t���X�^���̏������
        DirectX::BoundingFrustum initialFrustum_;
        DirectX::BoundingFrustum shadowInitialFrustum_;

        Vector2 mouseSensitivity_ = { 2.768f ,2.768f };//�J�������x

        //�J�����̊�����
        std::unique_ptr<FadeSystem> zoomParams_;

        //�J�����V�F�C�N
    public:
        struct  CameraShake
        {
            struct Position//�ʒu�ύX�ɂ��h��̃p�����[�^
            {
                float positionAmplitudeMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂���U���̏搔
                float positionFrequencyMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂�����g���̏搔
                DirectX::XMFLOAT3 positionAmplitude_ = { 1.0f,1.0f,1.0f };//�e���̐U��
                DirectX::XMFLOAT3 positionFrequency_ = { 1.0f,1.0f,1.0f };//�e���̎��g��
            }position_;

            struct Rotation
            {
                float rotationAmplitudeMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂���U���̏搔
                float rotationFrequencyMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂�����g���̏搔
                DirectX::XMFLOAT3 rotationAmplitude_ = { 1.0f,1.0f,1.0f };//�e���̐U�� 
                DirectX::XMFLOAT3 rotationFrequency_ = { 1.0f,1.0f,1.0f };//�e���̎��g�� 
            }rotation_;

            struct Timing//�h��̎������ԓ�
            {
                float duration_ = 1.0f;//�h��̎�������
                float blendInTime_ = 0;//���̗h��̃t�F�[�h�C���̎���
                float blendOutTime_ = 0;//���̗h��̃t�F�[�h�A�E�g�̎���
            }timing_;
        };
        struct  CameraShakeParameters
        {

            struct Position//�ʒu�ύX�ɂ��h��̃p�����[�^
            {
                float amplitudeMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂���U���̏搔
                float frequencyMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂�����g���̏搔
                Vector3 amplitude_ = { 1.0f,1.0f,1.0f };//�e���̐U��
                Vector3 frequency_ = { 1.0f,1.0f,1.0f };//�e���̎��g��
            }position_;

            struct Rotation
            {
                float amplitudeMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂���U���̏搔
                float frequencyMultiplier_ = 1.0f;//���ׂĂ̈ʒu�̗h��ɑ΂�����g���̏搔
                Vector3 amplitude_ = { 1.0f,1.0f,1.0f };//�e���̐U�� ������Z���̉�]�͔��f����Ȃ�
                Vector3 frequency_ = { 1.0f,1.0f,1.0f };//�e���̎��g�� ������Z���̉�]�͔��f����Ȃ�
            }rotation_;

            struct Timing//�h��̎������ԓ�
            {
                float duration_ = 1.0f;//�h��̎�������
                float blendInTime_ = 0.1f;//���̗h��̃t�F�[�h�C���̎���
                float blendOutTime_ = 0.1f;//���̗h��̃t�F�[�h�A�E�g�̎���
            }timing_;

            CameraShakeParameters operator=(const CameraShakeParameters& param);
        };

        //�J�����U���@���͏d���͂ł����P�̐U�������邾��
        void CameraShake(CameraShakeParameters shakePram);
        void CameraShake(std::string aseetName);
        void CameraShakeUpdate();

    private:
        bool activeCameraShake_ = false;

        Vector3 shakePosition_{ 0,0,0 };
        Vector4 shakeRotation_{ 0,0,0,1 };
        float shakeTimer_;
        Vector3 shakePosFreqTimer_;//���W�U���̎��g���̑��x��\�����߂̐��l
        Vector3 shakeRotFreqTimer_;//��]�U���̎��g���̑��x��\�����߂̐��l

        CameraShakeParameters shakeParam_;
        PerlinNoise shakePerlinNoise_;//�J�����U���p�̃m�C�Y

        bool imguiButton_ = false;
        private:
            //�J�����V�F�C�N�A�Z�b�g��
            void CameraShakeAssetCreation(CameraShakeParameters param,const std::string name);

        
    };
}


