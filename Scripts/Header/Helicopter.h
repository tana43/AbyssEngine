#pragma once
#include "Character.h"
#include "SkeletalMesh.h"

//戦闘ヘリ
class Helicopter : public AbyssEngine::Character
{
public:
    Helicopter() {}
    ~Helicopter() = default;

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    bool DrawImGui();



private:
    //プロペラは行列計算で回転させる
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    //モーターの回転出力
    float motorPower_ = 0.0f;


};

