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
        //���ׂẴA�j���[�V�������Ǘ����Ă���
        std::vector<Animation> animations_;
    };

}

