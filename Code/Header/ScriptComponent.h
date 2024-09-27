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
        void AddTerrainCollider();  //押し出し判定用コライダー
        std::shared_ptr<SphereCollider> AddAttackCollider(Vector3 localPos, float radius, std::string name = "AtkCollider", const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");   //攻撃判定用コライダー
        std::shared_ptr<SphereCollider> AddHitCollider(Vector3 localPos, float radius, std::string name = "HitCollider",const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");      //喰らい判定用コライダー

    public:
        const bool& GetActive() const { return active_; }
        void SetActive(const bool& active) { active_ = active; }

    private:
        bool active_ = true;
    };
}

