#pragma once
#include <memory>
#include "Component.h"
#include "MathHelper.h"

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
    private:
        void DebugCameraController();

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

        friend class RenderManager;

    private: 
        float debCameraSpeed_ = 10.0f;//�f�o�b�O���̃J�����X�s�[�h
        bool enableDebugController_ = true;//�f�o�b�O����I���A�I�t
    };
}


