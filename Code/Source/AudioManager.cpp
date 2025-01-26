#include "AudioManager.h"
#include "Misc.h"
#include "AudioSource.h"

using namespace AbyssEngine;

// ----- コンストラクタ -----
AudioManager::AudioManager()
{
    HRESULT result = S_OK;
    
    result = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    _ASSERT_EXPR(SUCCEEDED(result), HrTrace(result));

    result = xAudio2_->CreateMasteringVoice(&masterVoice_);
    _ASSERT_EXPR(SUCCEEDED(result), HrTrace(result));

    //音声読み込み
    LoadAudio();
}

void AbyssEngine::AudioManager::Update()
{
    //削除するイテレータのリスト
    std::vector<std::vector<std::weak_ptr<AudioSource>>::const_iterator> eraseList;

    for (auto itr = audioSourceList_.begin(); itr != audioSourceList_.end(); itr++)
    {
        if (const auto& p = itr->lock()) 
        {
            p->Update();
        }
        else
        {
            //存在しない物は削除する
            eraseList.emplace_back(itr);
        }
    }

    //削除するイテレータ
    //要素を戦闘から消してしまうと、消したいイテレーターと実際の配列にずれが生じるので末尾から消していく
    for (int i = eraseList.size() - 1;i >= 0;i--)
    {
        audioSourceList_.erase(eraseList[i]);
    }
}

// ----- BGM,SE 読み込み -----
void AudioManager::LoadAudio()
{
    // 音楽読み込み
    {
        // オーディオ読み込み
        {
            //audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/BGM/wolfgang_bgm1_2.wav", 1));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/BGM/Battle_Bgm_01.wav", 1));

            // SE: ビーム
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Beam/Beam_Always_01.wav", 30));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Beam/Beam_Shot_03.wav", 30));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Beam/Beam_Shot_01.wav", 100));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Beam/Beam_Hit_03.wav", 100));

            //発砲
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Gun/Shot_01.wav", 100));

            // SE: ミサイル
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Missile/Missile_Always.wav", 100));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Missile/Missile_Hit.wav", 100));

            // SE: 近接
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/MeleeAttack/Hit_01.wav", 10));

            // SE::Vitesse　システム周り
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Vitesse/Change_Target.wav", 10));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/System/System_Start_Up.wav", 1));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/System/Initialize_Always.wav", 1));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/System/Boost_Full.wav", 1));

            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/System/Alert.wav", 1));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/System/Pi.wav", 50));

            // SE:Vitesse　ブースト
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Vitesse/Boost_Always.wav", 1));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Vitesse/Boost_Burst_3.wav", 10));
            audioBuffer_.emplace_back(AudioBuffer(xAudio2_.Get(), L"./Assets/Audios/SE/Vitesse/Melee_Boost.wav", 3));


        }
    }

    // 音量調整
    {    
        // BGM音量調整
        {
            audioBuffer_[static_cast<int>(AudioIndex::Game)].SetVolume(0.2f);
        }

        // SE音量調整
        {
            audioBuffer_[static_cast<int>(AudioIndex::Beam_Always)].SetVolume(0.05f);
            audioBuffer_[static_cast<int>(AudioIndex::Beam_Shot_Vitesse)].SetVolume(0.10f);
            audioBuffer_[static_cast<int>(AudioIndex::Beam_Shot_Enemy)].SetVolume(0.01f);
            audioBuffer_[static_cast<int>(AudioIndex::Beam_Hit)].SetVolume(0.2f);


            audioBuffer_[static_cast<int>(AudioIndex::Gun_Shot)].SetVolume(0.05f);

            audioBuffer_[static_cast<int>(AudioIndex::Missile_Always)].SetVolume(0.2f);
            audioBuffer_[static_cast<int>(AudioIndex::Missile_Hit)].SetVolume(0.2f);

            audioBuffer_[static_cast<int>(AudioIndex::Melee_Hit_01)].SetVolume(0.38f);

            audioBuffer_[static_cast<int>(AudioIndex::Change_Target)].SetVolume(0.28f);

            audioBuffer_[static_cast<int>(AudioIndex::System_Start_Up)].SetVolume(0.8f);
            audioBuffer_[static_cast<int>(AudioIndex::System_Start_Up_Always)].SetVolume(0.3f);
            audioBuffer_[static_cast<int>(AudioIndex::Alert)].SetVolume(0.2f);
            audioBuffer_[static_cast<int>(AudioIndex::Pi)].SetVolume(0.2f);
            audioBuffer_[static_cast<int>(AudioIndex::System_Boost_Full)].SetVolume(0.8f);

            audioBuffer_[static_cast<int>(AudioIndex::Boost_Always)].SetVolume(0.13f);
            audioBuffer_[static_cast<int>(AudioIndex::Boost_Burst)].SetVolume(0.13f);
            audioBuffer_[static_cast<int>(AudioIndex::Melee_Boost)].SetVolume(0.1f);
        }
    }

}

const int AudioManager::Play(const AudioIndex& index, const bool isLoop, const bool isIgnoreQueue)
{
    return audioBuffer_[static_cast<int>(index)].Play(isLoop, isIgnoreQueue);
}

void AudioManager::Stop(const AudioIndex& index, const int& num)
{
    if (static_cast<int>(num) < 0)return;
    audioBuffer_[static_cast<int>(index)].Stop(num);
}

void AudioManager::StopAllAudio()
{
    for (auto& audio : audioBuffer_)
    {
        audio.AllStop();
    }
}

void AudioManager::SetVolume(const AudioIndex& index,const float& volume)
{
    audioBuffer_[static_cast<int>(index)].Volume(volume);
}

void AudioManager::SetVolume(const AudioIndex& index,const float& volume, const int& num)
{
    if (static_cast<int>(num) < 0)return;
    audioBuffer_[static_cast<int>(index)].Volume(volume,num);
}

// ----- SEを必要な数だけロードする -----
AudioManager::AudioBuffer::AudioBuffer(IXAudio2* xaudio2, const wchar_t* filename, const int& loadNum)
{
    // ロード数が無い
    if (loadNum <= 0) return;

    for (int i = 0; i < loadNum; ++i)
    {
        audios_.emplace_back(std::make_unique<Audio>(xaudio2, filename));
    }
}

// ----- 順番に再生する -----
const int AudioManager::AudioBuffer::Play(const bool isLoop,const bool isIgnoreQueue)
{
    // SEが無い
    if (audios_.size() == 0) return -1;
    if (counter_ >= audios_.size()) counter_ = 0;

    audios_.at(counter_)->Play(isLoop,isIgnoreQueue);

    return counter_++;
}

// ----- 指定の番号を停止する -----
void AudioManager::AudioBuffer::Stop(const int& num)
{
    // 0以下の数字は受け付けない
    if (num < 0) return;
    // サイズオーバーも受け付けない
    if (num >= audios_.size()) return;

    audios_.at(num)->Stop();
}

// ----- 全て停止する -----
void AudioManager::AudioBuffer::AllStop()
{
    // そもそもロードしていない
    if (audios_.size() == 0) return;

    for (auto& audio : audios_)
    {
        audio->Stop();
    }
}

// ----- 音量設定 -----
void AudioManager::AudioBuffer::Volume(const float& volume)
{
    float v = volume_ * volume;
    for (auto& audio : audios_)
    {
        audio->Volume(v);
    }
}

void AudioManager::AudioBuffer::Volume(const float& volume, const int& num)
{
    float v = volume_ * volume;
    audios_[num]->Volume(v);
}
