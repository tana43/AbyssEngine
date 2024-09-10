#pragma once
#include "Renderer.h"
#include "Animator.h"
#include "GltfSkeletalMesh.h"

namespace AbyssEngine
{
    class Actor;
    class Animator;
    class StaticMesh;

    class SkeletalMesh : public Renderer
    {
    public:
        SkeletalMesh() {}
        ~SkeletalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        

        //アニメーションアセットの追加
        //void AppendAnimation(const std::string& filename);
        //void AppendAnimations(const std::vector<std::string>& filenames);

        //アニメーションの再計算
        void RecalculateFrame()override;

        void DrawImGui()override;

        //アニメーションの再生
        void PlayAnimation(int animIndex,bool loop = true);

        //アニメーションの再生速度設定
        void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

        //ソケットにアタッチする　武器などを持たせることが出来る
        void SocketAttach(const std::shared_ptr<StaticMesh>& attachModel, const char* socketName);

        const Matrix& FindSocket(const char* socketName);

    public:
        GltfSkeletalMesh* GetModel() { return model_.get(); }

        const std::shared_ptr<Animator>& GetAnimator() { return animator_; }

        void SetOffsetRotation(const Vector3& rot) { offsetRot_ = rot; }

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;
        bool FrustumCulling(const DirectX::BoundingFrustum& frustum)override;
        bool ShadowCulling(const DirectX::BoundingBox& box)override;

    private:
        //ワールド行列
        Matrix worldMatrix_;

        //とりあえず描画させてみたいのでユニークポインタ
        //std::unique_ptr<FbxMeshData> model_;
        std::shared_ptr<GltfSkeletalMesh> model_;

        //animatorコンポーネント
        std::shared_ptr<Animator> animator_;

        Vector4 color_ = {1,1,1,1};

        size_t animationClip_ = 0;//再生中のアニメーション番号
        float timeStamp_ = 0.0f;//合計経過時間

        float animationSpeed_ = 1.0f;//アニメーション再生速度

        bool animationLoop_ = true;

        float blendWeight_ = 0.0f;//仮のモーションブレンドの重み

        //バウンディングボックス(フラスタムカリング用)
        Vector3 minValue_;
        Vector3 maxValue_;
        DirectX::BoundingBox boundingBox_;//AABB

        //オフセット回転値
        Vector3 offsetRot_ = {0,0,0};
    };
}

