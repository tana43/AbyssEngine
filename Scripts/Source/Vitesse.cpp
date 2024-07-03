#include "Vitesse.h"
#include "Actor.h"
#include "Animator.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Input.h"
#include "VitesseState.h"

using namespace AbyssEngine;

Vitesse::Vitesse() : HumanoidWeapon()
{
}

void Vitesse::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    //���f���ǂݍ���
    model_ = actor_->AddComponent<SkeletalMesh>("./Assets/Models/Vitesse/Vitesse_UE_01_Stand.glb");
    model_->GetAnimator()->AppendAnimations({
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_F.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_R.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_L.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Run_B.glb",
                //"./Assets/Models/Vitesse/Vitesse_UE_01_FrontFly.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Idle.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_F.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_R.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_L.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_B.glb",
                "./Assets/Models/Vitesse/Vitesse_UE_01_Fly_Up.glb"
        },
        {
            "Run_F",
            "Run_R",
            "Run_L",
            "Run_B",
            "Fly_Idle",
            "Fly_F",
            "Fly_R",
            "Fly_L",
            "Fly_B",
            "Fly_Up"
        });

    //�n��ړ�
    AnimBlendSpace2D rMoveAnim = AnimBlendSpace2D(model_.get(), "RunMove", static_cast<int>(AnimState::Stand),Vector2(0,0));
    //�O�A�E�A���A��̏��ɒǉ�
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_F), Vector2(0, 1));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_R), Vector2(1, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_L), Vector2(-1, 0));
    rMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Run_B), Vector2(0, -1));
    runMoveAnimation_ = model_->GetAnimator()->AppendAnimation(rMoveAnim);

    //�󒆈ړ�
    AnimBlendSpace2D fMoveAnim = AnimBlendSpace2D(model_.get(), "FlyMove", static_cast<int>(AnimState::Fly_Idle), Vector2(0, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_F), Vector2(0, 1));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_R), Vector2(1, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_L), Vector2(-1, 0));
    fMoveAnim.AddBlendAnimation(static_cast<int>(AnimState::Fly_B), Vector2(0, -1));
    flyMoveAnimation_ = model_->GetAnimator()->AppendAnimation(fMoveAnim);

    model_->GetAnimator()->PlayAnimation(static_cast<int>(AnimState::Run_Move));

    //�v���C���[�J�����ݒ�(�v���C���[�Ɛe�q�֌W��)
    //���͂��̂܂܃A�^�b�`���Ă��邪�A��X�Ǝ��̃J��������������
    const auto& c = Engine::sceneManager_->GetActiveScene().InstanceActor("Vitesse Camera");
    camera_ = c->AddComponent<Camera>();
    //c->SetParent(actor_);
    camera_->fov_ = DirectX::XMConvertToRadians(80.0f);
    camera_->targetOffset_ = Vector3(0.8f, 1.4f, 0);
    camera_->SetViewTarget(transform_.get());
    camera_->SetEnableDebugController(false);

    //�g�����X�t�H�[���ݒ�
    /*transform_->SetScaleFactor(7.0f);
    transform_->SetPosition(Vector3(0, 3.5f, 4.0f));*/

    //�J�����ݒ�
    camera_->armLength_ = 18.0f;
    camera_->targetOffset_ = { 12.2f,14.5f,0 };

    //�X�e�[�g�}�V���ݒ�
    stateMachine_ = std::make_unique<StateMachine<State<Vitesse>>>();
    stateMachine_->RegisterState(new VitesseState::GMoveState(this));
    stateMachine_->RegisterState(new VitesseState::FMoveState(this));
    stateMachine_->RegisterState(new VitesseState::TakeOffState(this));
    stateMachine_->RegisterState(new VitesseState::LandingState(this));
    stateMachine_->SetState(static_cast<int>(ActionState::GMove));
}

void Vitesse::Update()
{
    stateMachine_->Update();

    HumanoidWeapon::Update();

}

void Vitesse::Move()
{
    HumanoidWeapon::Move();

    //�㏸
    if (flightMode_ && Input::GameSupport::GetClimdButton())
    {
        Climb(climbSpeed_ * Time::deltaTime_);
    }


//#if 1
    //�u�����h�A�j���[�V������Weight�X�V
    Vector3 velocityXZ = {velocity_.x,0,velocity_.z};
    if (fabsf(velocityXZ.LengthSquared()) < 0.01f)
    {
        runMoveAnimation_->SetBlendWeight(Vector2(0,0));
        flyMoveAnimation_->SetBlendWeight(Vector2(0,0));
    }
    else
    {
        //�O�����Ɛi�s�����̍��̃x�N�g�����Z�o
        const auto& forward = transform_->GetForward();
        const auto& right = transform_->GetRight();
        Vector3 moveDirection;
        velocityXZ.Normalize(moveDirection);

        Vector2 result;

        //result.x = (forward.x * moveDirection.z + right.x * moveDirection.x);
        //result.y = forward.z * moveDirection.z + right.z * moveDirection.x;
        //result.Normalize();
        //result = result * (velocity_.Length() / Max_Speed);

        //���ςɂ��v�Z
        float dot = forward.Dot(moveDirection);
        float crossY = forward.z * moveDirection.x - forward.x * moveDirection.z;

        //���E����
        //���ϒl���P�̂Ƃ��ɂ��̂܂ܐ������Ђ�����Ԃ��Ă��܂��Ƒ傫���p�x���ς���Ă��܂��̂ŁA������l�����Čv�Z����
        if (crossY < 0)dot = DirectX::XM_PI - dot;
        result = { cosf(dot),sinf(dot) };
        result = result * (velocityXZ.Length() / Max_Horizontal_Speed);
        
        runMoveAnimation_->SetBlendWeight(result);
        flyMoveAnimation_->SetBlendWeight(result);
    }
//#else
//    runMoveAnimation_->SetBlendWeight((velocity_.Length() / Max_Speed) * 2);
//#endif // 1
//    flyMoveAnimation_->SetBlendWeight((velocity_.Length() / Max_Horizontal_Speed) * 2);

    CameraRollUpdate();
}

bool Vitesse::DrawImGui()
{
    HumanoidWeapon::DrawImGui();

    stateMachine_->DrawImGui();

    return true;
}

void Vitesse::UpdateInputMove()
{
    if (!camera_->isMainCamera_)return;

    //��s����Y���ɂ������悤�ɂ���
    if (flightMode_)
    {
        moveVec_ = camera_->ConvertTo3DVectorFromCamera(Input::GameSupport::GetMoveVector());
    }
    else
    {
        moveVec_ = camera_->ConvertTo2DVectorFromCamera(Input::GameSupport::GetMoveVector());
    }
}

void Vitesse::CameraRollUpdate()
{
    //���͒l�擾
    Vector2 input = Input::GameSupport::GetCameraRollVector();

    auto r = camera_->GetTransform()->GetRotation();
    const float rollSpeed = cameraRollSpeed_ * Time::deltaTime_;
    r.x = r.x + input.y * rollSpeed;
    r.y = r.y + input.x * rollSpeed;
    camera_->GetTransform()->SetRotation(r);
}