#include "AudioSource.h"
#include "Actor.h"
#include "AssetManager.h"
#include "Engine.h"
#include "Camera.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

AbyssEngine::AudioSource::~AudioSource()
{
    if (isDestroyedAudioStop_)
    {
        Stop();
    }
}

void AudioSource::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    // マネージャーに登録
    Engine::audioManager_->audioSourceList_.emplace_back(std::static_pointer_cast<AudioSource>(shared_from_this()));

    //初期化
    for (int i = 0; i < Regist_Count; i++)
    {
        playingAudios_[i] = -1;
    }
}

void AbyssEngine::AudioSource::Update()
{
    UpdateDistanceAttenuation();
}

void AbyssEngine::AudioSource::DrawImGui()
{
    using namespace ImGui;
    if (TreeNode("AudioSource"))
    {
        int sai = static_cast<int>(assetAudioIndex_);
        InputInt("Asset Audio Index", &sai);

        Checkbox("Loop", &isLoop_);
        DragFloat("Volume",&volume_,0.01f);

        Checkbox("Distance Attenuation", &activeDistanceAttenuation_);
        if (activeDistanceAttenuation_)
        {
            DragFloat("Range Of Sound",&rangeOfSound_,0.1f);
        }

        if (Button("Play"))
        {
            Play();
        }

        if (Button("Stop"))
        {
            Stop();
        }

        TreePop();
    }
}

void AbyssEngine::AudioSource::Play()
{
    // 再生
    const int audio = Engine::audioManager_->Play(assetAudioIndex_,isLoop_);

    // 再生した音声を登録
    playingAudios_[registCounter_] = audio;

    //登録する要素数を更新
    registCounter_++;
    if (registCounter_ >= Regist_Count)
    {
        registCounter_ = 0;
    }

    //一度距離判定
    UpdateDistanceAttenuation();
}

void AbyssEngine::AudioSource::Stop()
{
    for (auto& audio : playingAudios_)
    {
        Engine::audioManager_->Stop(assetAudioIndex_, audio);
    }
}

void AbyssEngine::AudioSource::UpdateDistanceAttenuation()
{
    // 距離減衰のオンオフ
    if (activeDistanceAttenuation_)
    {
        // リスナーであるカメラと距離判定
        const std::shared_ptr<Camera>& listener = Camera::GetMainCamera();

        if (!listener)return;

        // 距離を判定して音を減衰させる
        float distSq = Vector3::DistanceSquared(transform_->GetPosition(), listener->GetEye());

        //減衰率
        float decayRate;
        if (distSq != 0)
        {
            decayRate = (rangeOfSound_ * rangeOfSound_) / distSq;
            decayRate = std::clamp(decayRate, 0.0f, 1.0f);
        }
        else
        {
            decayRate = 1.0f;
        }

        //音量を算出
        float volume = volume_ * decayRate;

        //再生している音に代入
        for (auto& audio : playingAudios_)
        {
            Engine::audioManager_->SetVolume(assetAudioIndex_, volume, audio);
        }
    }
    else
    {
        //そのままのボリュームを設定
        for (auto& audio : playingAudios_)
        {
            Engine::audioManager_->SetVolume(assetAudioIndex_, volume_, audio);
        }
    }
}
