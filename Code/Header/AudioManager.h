#pragma once
#include "Audio.h"
#include <wrl.h>
#include <memory>
#include <xaudio2.h>
#include <vector>

namespace AbyssEngine
{
    class AudioSource;

    enum class AudioIndex
    {
        //----------BGM-------------
        Game,       // ゲーム

        //----------SE--------------
        Beam_Always,
        Beam_Shot_Vitesse,
        Beam_Shot_Enemy,
        Beam_Hit,

        Gun_Shot,

        Missile_Always,
        Missile_Hit,

        Melee_Hit_01,

        Change_Target,
        System_Start_Up,
        System_Start_Up_Always,
        System_Boost_Full,
        
        //ピーピーピー
        Alert,

        Pi,

        Boost_Always,
        Boost_Burst,
        Melee_Boost,

        Max,
    };

    class AudioManager
    {
    public:
        AudioManager();
        ~AudioManager() = default;


        // 更新処理
        void Update();

        void LoadAudio(); // 音楽読み込み

        // 再生
        const int Play(
            const AudioIndex& index,
            const bool isLoop = false,
            const bool isIgnoreQueue = false
        );


        void Stop(const AudioIndex& index, const int& num);      // SE停止

        void StopAllAudio();                            // 全音楽停止

        void SetVolume(const AudioIndex& index, const float& volume);
        void SetVolume(const AudioIndex& index, const float& volume,const int& num);

    public:
        Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
        IXAudio2MasteringVoice* masterVoice_ = nullptr;
        std::vector<std::weak_ptr<AudioSource>> audioSourceList_;

    private:
        struct AudioBuffer
        {
        public:
            AudioBuffer(IXAudio2* xaudio2, const wchar_t* filename, const int& loadNum = 1);
            [[nodiscard]] const int Play(const bool isLoop = false, const bool isIgnoreQueue = false);
            void Stop(const int& num);
            void AllStop();
            void Volume(const float& volume);
            void Volume(const float& volume,const int& num);
            
            // 初期音量
            void SetVolume(const float& volume) { volume_ = volume; }

        private:
            std::vector<std::unique_ptr<Audio>> audios_;
            int     counter_ = 0;
            float   volume_ = 1.0f;
        };
        std::vector<AudioBuffer> audioBuffer_;

    };

}

