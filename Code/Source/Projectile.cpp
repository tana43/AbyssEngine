#include "Projectile.h"
#include "Actor.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "SphereCollider.h"
#include "GameCollider.h"
#include "SceneManager.h"
#include "StageManager.h"
#include "Stage.h"
#include "Character.h"

using namespace AbyssEngine;

void Projectile::Initialize(const std::shared_ptr<Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    atkCollider_ = AddAttackCollider(Vector3::Zero, radius_);
    //collider_ = actor->AddComponent<SphereCollider>();
}

void Projectile::Update()
{
    if (isHoming_)
    {
        //ホーミング更新
        HomingUpdate();
    }
    
    //移動更新
    MoveUpdate();
    
    //地形処理
    IsTerrainHitUpdate();


#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, Vector4(1, 0, 0, 1));
#endif // _DEBUG

    LifeTimeUpdate();
}

void AbyssEngine::Projectile::SetDirection(const Vector3& dir)
{
    //前方向算出　方向と被らないようにすこし真上からずらす
    Vector3 forward = transform_->GetForward();
    forward.y += 0.00001f;

    //回転軸算出
    Vector3 axis = forward.Cross(dir);
    axis.Normalize();

    //回転量算出
    float angle = acosf(std::clamp(forward.Dot(dir),-1.0f,1.0f));

    //回転行列算出
    Matrix R = transform_->GetRotateMatrix() * Matrix::CreateFromAxisAngle(axis, angle);

    //クォータニオンに変換
    Quaternion q = DirectX::XMQuaternionRotationMatrix(R);
    
    transform_->SetRotation(q.To_Euler());
}

void AbyssEngine::Projectile::SetRadius(const float& radius)
{
    radius_ = radius;
    atkCollider_->SetRadius(radius);
}

void AbyssEngine::Projectile::LifeTimeUpdate()
{
    //寿命処理
    lifespan_ -= actor_->GetDeltaTime();
    if (lifespan_ <= 0)
    {
        actor_->Destroy(actor_);
    }
}

void AbyssEngine::Projectile::MoveUpdate()
{
    //飛び道具の射出距離
    //const Vector3& pos = transform_->GetPosition();
    //const Vector3& velo = direction_ * speed_ * actor_->GetDeltaTime();
    //transform_->SetPosition(pos + velo);

    const Vector3& pos = transform_->GetPosition();
    const Vector3& velo = transform_->GetForward() * speed_ * actor_->GetDeltaTime();
    transform_->SetPosition(pos + velo);

}

void AbyssEngine::Projectile::HomingUpdate()
{
    //ホーミング可能か
    if (homingExpired_)return;

    //ターゲットを検索
    std::shared_ptr<Transform> target;
    if (!(target = targetTransform_.lock()))
    {
        //登録されているターゲットがいないならタグ検索
        target = Engine::sceneManager_->GetActiveScene().FindByTag(targetTag_).lock()->GetTransform();
    }

    //ターゲットがいないなら処理しない
    if (!target)return;

    const Vector3 pos = transform_->GetPosition();
    Vector3 targetPos = target->GetTransform()->GetPosition();
    Vector3 vec = targetPos - pos;
    float qDist = vec.LengthSquared();

    //ターゲットがキャラクターを持っているなら、1秒先の座標を予測させる
    if (const auto& parent = target->GetActor()->GetParent().lock())
    {
        if (const auto& targetChara = parent->GetComponent<Character>())
        {
            //おおよそターゲットまで移動するのに何秒掛かるかを算出
            //ホーミングの軌道を考慮しないので、どうしても短めの時間になってしまう
            float t = qDist / (speed_ * speed_);

            t = std::clamp(t, 0.0f, 1.5f);

            //到達時間までが少ないときはターゲット位置にホーミングする
            //if (t < 0.3f)
            //{
            //    //そのままでいいのでなにもしない
            //}
            //else
            {
                //t秒後の座標を算出
                const Vector3 velo = targetChara->GetVelocity();
                const Vector3 targetPosFuture = targetPos + velo * t;
                targetPos = targetPosFuture;
            }

            Homing(targetPos);
        }
    }
    else
    {
        //キャラクターのコンポーネントがなければ前と同じ処理
        Homing(targetPos);
    }

    //距離が近くて、進行方向と敵とのベクトルの内積値が負なら追尾しない
    //距離判定
    if (qDist < homingExpiredLength_ * homingExpiredLength_)
    {
        Vector3 dirToTarget;
        vec.Normalize(dirToTarget);
        if (dirToTarget.Dot(transform_->GetForward()) < 0)
        {
            homingExpired_ = true;
        }
    }
}

void AbyssEngine::Projectile::IsTerrainHitUpdate()
{
    if (isHoming_)
    {
        //50以上のときは地面に当たることはほぼ無いので
        if (transform_->GetPosition().y > 50.0f)return;

        //ホーミングは毎フレーム判定処理する必要がある
        if (const auto& stage = Engine::stageManager_->GetActiveStage().lock())
        {
            const Vector3 start = transform_->GetPosition();
            const float speed = speed_ * actor_->GetDeltaTime();
            const Vector3 end = transform_->GetPosition() + transform_->GetForward() * speed;
            Vector3 hitPosition, hitNormal;

            //レイキャスト
            if (stage->RayCast(start, end,hitPosition,hitNormal))
            {
                //当たったなら削除する
                 
                //座標を地形が当たった位置へ
                transform_->SetPosition(hitPosition);

                HitTerrain();

                Actor::Destroy(actor_);
            }
        }
    }
    else
    {
        //地形にあたらないなら処理しない
        if (!isTerrainHit_)return;

        //地面に当たるところまでのベクトルと、進行方向で内積
        Vector3 toHit = terrainHitPos_ - transform_->GetPosition();
        toHit.Normalize();

        float dot = toHit.Dot(transform_->GetForward());

        //内積値が負なら通り過ぎているのでDestroyする
        if (dot < 0)
        {
            //座標を地形が当たった位置へ
            transform_->SetPosition(terrainHitPos_);

            HitTerrain();

            Actor::Destroy(actor_);
        }
    }
}

void AbyssEngine::Projectile::Homing(Vector3 targetPos)
{
    const Vector3 pos = transform_->GetPosition();
    const Vector3 forward = transform_->GetForward();
    Vector3 dir = targetPos - pos;
    dir.Normalize();

    //必要な回転角を算出
    float rad = acosf(std::clamp(forward.Dot(dir), -1.0f, 1.0f));

    //回転速度算出
    float rotSpeed = homingStrength_ * actor_->GetDeltaTime();
    if (rotSpeed > rad)
    {
        rotSpeed = rad;
    }

    const Vector3 cross = forward.Cross(dir);
 
    //任意軸で回転行列作成
    Matrix AR = Matrix::CreateFromAxisAngle(cross, rotSpeed);

    //ホーミング後の回転行列を算出し、代入
    Matrix R = transform_->GetRotateMatrix() * AR;

    Quaternion q = Quaternion::CreateFromRotationMatrix(R);
    transform_->SetRotation(q.To_Euler());
}
