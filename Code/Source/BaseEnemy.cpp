#include "BaseEnemy.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "SceneManager.h"

using namespace AbyssEngine;

void BaseEnemy::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //������
    Character::Initialize(actor);

    //�G�l�~�[�^�O�t��
    Character::ReplaceTag(Character::Tag_Enemy);

    //�e���g���[�����l�ݒ�
    territoryOrigin_ = transform_->GetPosition();

    //�e�s�����\��Ԃ�
    canAttack_ = true;
    canDodge_ = false;

    //�^�[�Q�b�g�擾
    const auto& target = Engine::sceneManager_->GetActiveScene().Find("Player");
    if (const auto& t = target.lock())
    {
        targetActor_ = t;
    }
}

void BaseEnemy::UpdateBegin()
{
    //�ړ��x�N�g�������Z�b�g
    moveVec_ = {};
}

void BaseEnemy::DrawDebug()
{
#if _DEBUG
    // �꒣��͈͂��f�o�b�O�~���`��
    Engine::renderManager_->debugRenderer_->DrawCylinder(territoryOrigin_, territoryRange_, 0.5f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
#endif // _DEBUG
}

bool BaseEnemy::MoveToTarget()
{
    Vector3 toTarget = targetPosition_ - transform_->GetTransform()->GetPosition();
    Vector3 moveVec = toTarget;
    moveVec.y = 0.0f;
    moveVec.Normalize();
    moveVec_ = moveVec;

    //�ړI�n�ɂ��Ă��邩����
    float distSq = toTarget.LengthSquared();
    float radiusSq = goalRadius_ * goalRadius_;
    if (distSq < radiusSq)
    {
        return true;
    }

    return false;
}

void BaseEnemy::SideMove(const AbyssEngine::Vector3& centerPos ,bool& moveRight)
{
    //�ǂɓ������Ă���Ȃ甽�Ε�����
    if (hitWall_)
    {
        moveRight = !moveRight;
    }

    //�����̍��W���猩�ĉ��֓���
    //�O�ς��g�p
    Vector3 toCenter = centerPos - transform_->GetPosition();
    toCenter.Normalize();
    //���E�ǂ���Ɉړ����邩�ɍ��킹�ĊO�ςɎg�p����x�N�g����ύX  
    const Vector3 ud = moveRight ? Vector3(0, -1, 0) : Vector3(0, 1, 0);
    const Vector3 cross = toCenter.Cross(ud);

    moveVec_ = cross;
}

void BaseEnemy::SetRandomTargetPosition()
{
    float theta = Math::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Math::RandomRange(0.0f, territoryRange_);
    float a = cosf(theta);
    targetPosition_.x = territoryOrigin_.x + sinf(theta) * range;
    targetPosition_.y = territoryOrigin_.y;
    targetPosition_.z = territoryOrigin_.z + cosf(theta) * range;
}

void BaseEnemy::SetTerritory(const AbyssEngine::Vector3& origin, float range)
{
    territoryOrigin_ = origin;
    territoryRange_ = range;
}
