#pragma once
#include "ScriptComponent.h"
#include "StaticMesh.h"
#include "MeshCollider.h"

//ステージを管理する基底クラス
//空間分割に対応したレイキャスト用
namespace AbyssEngine
{
    class Stage : public AbyssEngine::ScriptComponent
    {
    public:
        Stage() {}
        ~Stage() { meshColliders_.clear(); }

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);

        //void Update()override;
        void DrawDebug()override;
        void DrawImGui()override;

        struct Area
        {
            DirectX::BoundingBox	boundingBox;
            std::vector<int>		triangleIndices;
        };
        std::vector<Area> areas;

        bool RayCast(
            const AbyssEngine::Vector3& start,
            const AbyssEngine::Vector3& end,
            AbyssEngine::Vector3& hitPosition,
            AbyssEngine::Vector3& hitNormal);

        bool SphereCast(
            const AbyssEngine::Vector3& origin,
            const AbyssEngine::Vector3& direction,
            float radius,
            float& distance,
            AbyssEngine::Vector3& hitPosition,
            AbyssEngine::Vector3& hitNormal,
            bool drawDebug = false
        );

        //ステージに配置するアクターを追加
        std::shared_ptr<AbyssEngine::Actor> AddStageModel(const std::string& actorName, std::string modelPath);

        //登録されているすべてのモデル頂点から三角形を登録していく
        void RegisterTriangles();
    protected:

        //空間分割
        void SpaceDivision();

        //配置するモデルを登録しておくためのベクター変数
        std::vector<std::weak_ptr<AbyssEngine::MeshCollider>> meshColliders_;

        //各モデルの三角形をまとめて管理
        std::vector <AbyssEngine::Collider::Triangle > triangles_;

        //空間分割オン、オフ
        bool spaceDivisionEnabled_ = false;

        //現在レイが飛ばされたエリア
        int currentArea_;

#if _DEBUG
        int showTriangleCount_ = 2000;
#endif // _DEBUG

        friend class StageManager;
    };
}

