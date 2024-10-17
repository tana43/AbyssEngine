#pragma once
#include "Component.h"
#include "CollisionHelper.h"
#include "MathHelper.h"

#include <string>

namespace AbyssEngine
{
    class Collider;
    class SphereCollider;
    class SkeletalMesh;
    class AttackCollider;
    class HitCollider;
    class TerrainCollider;

    //独自のコンポーネントを開発する際に継承元として使用するコンポーネント
    //Update処理を持っている
    class ScriptComponent : public Component
    {
    public:
        ScriptComponent() {}
        ~ScriptComponent() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //何かに当たっている場合自動で呼ばれる
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result) {};

        virtual void UpdateBegin() {}
        virtual void UpdateBefore() {}
        virtual void Update() {}
        virtual void UpdateAfter() {}
        virtual void UpdateEnd() {}

        //コライダーを付ける
        std::shared_ptr<TerrainCollider> AddTerrainCollider(Vector3 localPos, float radius, std::string name = "TerrainCollider", const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");  //押し出し判定用コライダー
        std::shared_ptr<AttackCollider> AddAttackCollider(Vector3 localPos, float radius, std::string name = "AtkCollider", const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");   //攻撃判定用コライダー
        std::shared_ptr<HitCollider> AddHitCollider(Vector3 localPos, float radius, std::string name = "HitCollider",const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");      //喰らい判定用コライダー
    
    private:
        void AddColliderCommon(const std::shared_ptr<SphereCollider>& colliderCom ,Vector3 localPos, float radius, const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");
    public:
        const bool& GetActive() const { return active_; }
        void SetActive(const bool& active) { active_ = active; }

    protected:
        bool active_ = true;
    };
}

