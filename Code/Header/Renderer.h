#pragma once
#include "Component.h"

namespace AbyssEngine
{
    //レンダリングされるコンポーネントの基礎クラス
    class Renderer : public Component 
    {
    public:
        void SetEnable(bool value);
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    protected:
        bool isCalled_ = false;     //既にマネージャーに登録されているか
        bool enabled_ = true;       //描画するか
        bool enabledOld_ = false;   //変更時トリガー用キャッシュ
        bool canRender_ = false;    //レンダリング可能か（データが入っていないか)


        //int subsetCount_ = 0; //サブセットの数
    private:
        virtual void Render(const int subsetNumber = 0) {}          //描画実行
        virtual void RenderShadow(const int subsetNumber = 0) {}    //シャドウマップ描画実行
        virtual void RecalculateFrame() {};                         //頂点データを再計算
    };
}


