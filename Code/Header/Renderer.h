#pragma once
#include "Component.h"
#include "MathHelper.h"
#include "Shader.h"
#include <DirectXCollision.h>

namespace AbyssEngine
{
    //レンダリングされるコンポーネントの基礎クラス
    class Renderer : public Component 
    {
    public:
        Renderer() {}
        ~Renderer() {}

    public:
        void SetFilePath(const char* path) { filePath_ = path; }

        void SetEnable(bool value);
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    protected:
        bool isCalled_ = false;     //既にマネージャーに登録されているか
        bool enabled_ = true;       //描画するか
        bool canRender_ = false;    //レンダリング可能か（データが入っていないか)

        std::string filePath_;

        //int subsetCount_ = 0; //サブセットの数
    private:
        //virtual void Render(const int subsetNumber = 0) {}          //描画実行
        virtual void Render() = 0;          //描画実行
        virtual void RenderShadow() {}    //シャドウマップ描画実行
        virtual void RecalculateFrame() {}                         //頂点データを再計算
        virtual bool FrustumCulling(const DirectX::BoundingFrustum& frustum) { return true; }//フラスタムカリング

        friend class RenderManager;
    };
}


