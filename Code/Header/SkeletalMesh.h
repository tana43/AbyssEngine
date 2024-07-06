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

        bool DrawImGui()override;

        //アニメーションの再生
        void PlayAnimation(int animIndex,bool loop = true);

        //アニメーションの再生速度設定
        void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

        GltfSkeletalMesh* GetModel() { return model_.get(); }

        std::shared_ptr<Animator>& GetAnimator() { return animator_; }

        //ソケットにアタッチする　武器などを持たせることが出来る
        void SocketAttach(const std::shared_ptr<StaticMesh>& attachModel, const char* socketName);

        DirectX::XMFLOAT4X4& FindSocket(const char* socketName);

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;

    private:

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
    };
}

