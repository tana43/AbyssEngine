#pragma once
#include "Component.h"
#include "AudioManager.h"

#include <string>

namespace AbyssEngine
{
    class AudioSource : public Component
    {
    public:
        AudioSource() {}
        ~AudioSource();

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        void Update();

        void DrawImGui()override;

        void Play();

        void Stop();

    private:
        //距離減衰更新
        void UpdateDistanceAttenuation();

    public:
        void SetAssetAudioIndex(const AudioIndex& index) { assetAudioIndex_ = index; }
        void SetVolume(const float& v) { volume_ = v; }
        void SetIsLoop(const bool& loop) { isLoop_ = loop; }
        void SetActiveDistanceAttenuation(const bool& active) { activeDistanceAttenuation_ = active; }
        void SetRangeOfSound(const float& range) { rangeOfSound_ = range; }
        void SetIsDestroyedAudioStop(const bool& active) { isDestroyedAudioStop_ = active; }

    private:
        // オーディオ自分が再生するオーディオ番号
        AudioIndex assetAudioIndex_;

        //　管理できる音声の数
        static constexpr int Regist_Count = 10;

        // 自分が再生している音声の要素番号
        int playingAudios_[Regist_Count];

        //　次に登録する要素数
        int registCounter_ = 0;

        // 音量
        float volume_ = 1.0f;

        // どこまでの距離に届く音声か
        float rangeOfSound_ = 10.0f;

        // ループさせるか
        bool isLoop_ = false;

        // 距離減衰をさせるか
        bool activeDistanceAttenuation_ = true;

        //アクターの消滅と同時に音も消すか
        bool isDestroyedAudioStop_ = false;
    };
}

