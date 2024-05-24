#pragma once
#include "SkeletalMesh.h"
#include "Animation.h"
#include "Component.h"

namespace AbyssEngine
{
    class Animator final : public Component
    {
    public:
        Animator() {}
        ~Animator() {}


    private:
        //すべてのアニメーションを管理している
        std::vector<Animation> animations_;
    };

}

