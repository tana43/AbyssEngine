#pragma once
#include <vector>

namespace AbyssEngine
{
    //数値のフェードイン、アウトができるテンプレートクラス
    class FadeSystem
    {
    public:
        //フェードさせたい数値を登録
        FadeSystem(float* data);
        FadeSystem(float* data, size_t arraySize);
        ~FadeSystem() {}

        //フェードする 遷移が完了していればtrue
        //毎フレーム呼び出すこと
        bool Fade(const float& time, const float& nextParam);
        bool Fade(const float& time, const std::vector<float>& nextParams);

    private:
        std::vector<float*> params_;

        //フェード開始時の値を格納する
        std::vector<float> originParams_;

        //遷移中か
        bool transitioning_ = false;

        float timer_;
        float time_;
    };
}

