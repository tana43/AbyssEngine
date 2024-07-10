#include "Animation.h"
#include "SkeletalMesh.h"
#include "Engine.h"
#include "Animator.h"

#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

Animation::Animation(SkeletalMesh* model, const std::string& name_, const int& index, bool loop) :
    name_(name_),animIndex_(index),loopFlag_(loop)
{
    animatedNodes_ = model->GetAnimator()->GetAnimatedNodes();
}

//Animation::Animation(SkeletalMesh* model, const std::string& name_, bool loop) : 
//    name_(name_), loopFlag_(loop)
//{
//    animatedNodes_ = model->GetAnimator()->GetAnimatedNodes();
//}

void Animation::Initialize()
{
    timeStamp_ = 0.0f;
}

std::vector<GeometricSubstance::Node> Animation::UpdateAnimation(GltfSkeletalMesh* model)
{
    UpdateTime();
    model->Animate(animIndex_, timeStamp_, animatedNodes_, loopFlag_);

    return animatedNodes_;
}

void Animation::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }

        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

void Animation::UpdateTime()
{
    timeStamp_ += animSpeed_ * Time::deltaTime_;
}

AnimBlendSpace1D::AnimBlendSpace1D(SkeletalMesh* model, const std::string& name_, const int& index0, const int& index1)
    : Animation(model, name_, index0, true) 
{
    blendAnimNodes_[0] = blendAnimNodes_[1] = animatedNodes_;
    blendAnimDatas_.emplace_back(BlendAnimData(index0, 0.0f));
    blendAnimDatas_.emplace_back(BlendAnimData(index1, 1.0f));
}

AnimBlendSpace1D::AnimBlendSpace1D(SkeletalMesh* model, AnimBlendSpace1D animData) : Animation(model,animData.name_,animData.animIndex_,true)
{
    blendAnimNodes_[0] = animData.blendAnimNodes_[0];
    blendAnimNodes_[1] = animData.blendAnimNodes_[1];
    blendAnimDatas_ = animData.blendAnimDatas_;
}

//���݂̃u�����h�̏d�݂��琳�ɋ߂����[�V�����ƁA���ɋ߂����[�V�����̓���擾���A�u�����h����
std::vector<GeometricSubstance::Node> AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model)
{
    UpdateTime();

    //�u�����h�̑��x����
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const float blendSpeed = std::clamp(blendWeight_ - lastBlendWeight_, -blendMaxSpeed, blendMaxSpeed);

    //���ۂ̃u�����h�l�̌v�Z
    const float nextBlendWeight = std::clamp(lastBlendWeight_ + blendSpeed, minWeight_, maxWeight_);

    //�u�����h����Q�̃��[�V��������� blendWeight��[0]��[1]��weight�l�Ɏ��܂�͈͂̃A�j���[�V������T��
    BlendAnimData blendAnims[2] = {blendAnimDatas_[0],blendAnimDatas_[1]};
    if (blendAnimDatas_.size() >= 2)
    {
        for (int i = 2; i < blendAnimDatas_.size(); i++)
        {
            if (blendAnimDatas_[i].weight_ < nextBlendWeight)
            {
                if (blendAnimDatas_[i].weight_ > blendAnims[0].weight_)
                {
                    blendAnims[0] = blendAnimDatas_[i];
                }
            }
            else
            {
                if (blendAnimDatas_[i].weight_ < blendAnims[1].weight_)
                {
                    blendAnims[1] = blendAnimDatas_[i];
                }
            }
        }
    }
    
    //��̃��[�V�����f�[�^������ۂɕ⊮���Ɏg����l���v�Z
    //�}�ȃu�����h�̏d���̕ω���h���悤�ɑO��̒l����ϓ�����l�ɏ����݂���
    const float maxBlendWeight = blendAnims[1].weight_ - blendAnims[0].weight_;
    const float blendWeight = std::clamp((nextBlendWeight - blendAnims[0].weight_) / maxBlendWeight
                                        ,0.0f,1.0f);

    //���[�V�����u�����h
    model->Animate(blendAnims[0].index_, timeStamp_, blendAnimNodes_[0]);
    model->Animate(blendAnims[1].index_, timeStamp_, blendAnimNodes_[1]);
    model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], blendWeight, animatedNodes_);

    lastBlendWeight_ = nextBlendWeight;

    return animatedNodes_;
}

void AnimBlendSpace1D::AddBlendAnimation(const int& index, const float& weight)
{
    BlendAnimData anim = { index,weight };
    blendAnimDatas_.emplace_back(anim);
}

void AnimBlendSpace1D::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }
        ImGui::SliderFloat("Blend Weight", &blendWeight_, minWeight_, maxWeight_);
        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

AnimBlendSpace2D::AnimBlendSpace2D(SkeletalMesh* model, const std::string& name_, const int& index,Vector2 weight)
    : Animation(model,name_,index)
{
    //��������n����鏉�����[�V�����̂ݓo�^
    blendAnimDatas_.emplace_back(BlendAnimData(index,weight));

    for(auto& animNode : blendAnimNodes_)
    {
        animNode = animatedNodes_;
    }
    secondBlendAnimNodes_ = animatedNodes_;
}

std::vector<GeometricSubstance::Node> AnimBlendSpace2D::UpdateAnimation(GltfSkeletalMesh* model)
{
    UpdateTime();

    //�u�����h�̑��x����
    const float blendMaxSpeed = 5.0f * Time::deltaTime_;
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //�u�����h�l�̎Z�o
    Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };
    
    //��������̋������P�ȏ㗣��Ă���ꍇ�́A�������|���Ă���
    //weightLength�̓u�����h���Ƃ��čŌ�̕��Ŏg��
    float weightLength = nextBlendWeight.Length();
    if (weightLength > 1.0f)
    {
        //�l�𐳋K�����������P�ɗ}����
        nextBlendWeight.Normalize();
        weightLength = 1.0f;
    }
    lastBlendWeight_ = nextBlendWeight;

    //�����̃u�����h�l���ǂ��Ɉʒu���邩�𔻒肵�A�����Ɉʒu����ҋ@���[�V�����Ɨ��ׂ̂Q�̈ړ����[�V�����Ƃ�
    // �u�����h�A�ő���̃u�����h����ŏI�I�ȃ��[�V�������Z�o����
    //�C���[�W
    //  * : �e�A�j���[�V����
    //  + : �u�����h�l
    //                           |
    //                           *(0,1)
    //                           |      (0.7,0.5) ���Ɖ��̃��[�V�������u�����h���A����ɂ���ƒ����̑ҋ@���[�V�������u�����h
    //                           |      +
    //               (-1,0)      |          (1,0)
    //----------------*----------*----------*------------------
    //                           |
    //                           |
    //                           |
    //                           *(0,-1)
    //                           |

    //�܂����ׂ̃��[�V�������擾����
    //���v���ɂȂ�悤�ȏ��Ԃŗv�f������
    BlendAnimData animDatas[2];
    BlendSituation blendSituation;
    {

        //�ǂ̒��x�u�����h������ׂ���Ԃ������肷��
        if (nextBlendWeight.x == 0 && nextBlendWeight.y == 0)
        {
            blendSituation = BlendSituation::None;
        }
        else if (nextBlendWeight.x == 0)
        {
            blendSituation = BlendSituation::Once;
            //���E�͂Ȃ��̂őO��ǂ��炩
            if (nextBlendWeight.y > 0)animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_F)];
            else animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_B)];
        }
        else if (nextBlendWeight.y == 0)
        {
            blendSituation = BlendSituation::Once;
            //�O��͂Ȃ��̂ō��E�ǂ��炩
            if (nextBlendWeight.x > 0)animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_R)];
            else animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_L)];
        }
        else//����ȍ~�͕K���Q��u�����h����K�v������ꍇ�ɂȂ�
        {
            blendSituation = BlendSituation::Twice;

            //x>0,y>0���玞�v���ɔ��肵�Ă���
            if (nextBlendWeight.x > 0 && nextBlendWeight.y > 0)
            {
                animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_F)];
                animDatas[1] = blendAnimDatas_[static_cast<int>(State::Move_R)];
            }
            else if (nextBlendWeight.x > 0 && nextBlendWeight.y < 0)
            {
                animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_R)];
                animDatas[1] = blendAnimDatas_[static_cast<int>(State::Move_B)];
            }
            else if (nextBlendWeight.x < 0 && nextBlendWeight.y < 0)
            {
                animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_B)];
                animDatas[1] = blendAnimDatas_[static_cast<int>(State::Move_L)];
            }
            else
            {
                animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_L)];
                animDatas[1] = blendAnimDatas_[static_cast<int>(State::Move_F)];
            }
        }
    }

    //���ۂɃ��[�V�������u�����h���Ă���
    switch (blendSituation)
    {
    case BlendSituation::None:
        //�������Ȃ��Ă����̂őҋ@���[�V�������Đ�
        model->Animate(
            blendAnimDatas_[static_cast<int>(State::Idle)].index_,
            timeStamp_,
            animatedNodes_);
        break;

    case BlendSituation::Once:
        //BlendSpace1D�Ɠ������[�V�����u�����h
        model->Animate(blendAnimDatas_[static_cast<int>(State::Idle)].index_,timeStamp_, blendAnimNodes_[0]);
        model->Animate(animDatas[0].index_, timeStamp_, blendAnimNodes_[1]);
        model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], weightLength, animatedNodes_);
        break;

    case BlendSituation::Twice:
        //�΂߈ړ����[�V����

        //�P��ڂ̃u�����h�l�̎Z�o
        //2�̃��[�V�����f�[�^���猩���{���̃u�����h�l�܂ł̋����̔���g��
        float _weightLength[2] =
        {
            Vector2(animDatas[0].weight_ - nextBlendWeight).Length(),
            Vector2(animDatas[1].weight_ - nextBlendWeight).Length(),
        };
        float firstWeight = _weightLength[0] / (_weightLength[0] + _weightLength[1]);

        model->Animate(animDatas[0].index_, timeStamp_, blendAnimNodes_[0]);
        model->Animate(animDatas[1].index_, timeStamp_, blendAnimNodes_[1]);
        model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], firstWeight, secondBlendAnimNodes_);
        model->Animate(blendAnimDatas_[static_cast<int>(State::Idle)].index_, timeStamp_, blendAnimNodes_[0]);
        model->BlendAnimations(blendAnimNodes_[0], secondBlendAnimNodes_, weightLength, animatedNodes_);
        break;
    }

    return animatedNodes_;

    //�v
#if 0
    //�u�����h�̑��x����
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //���ۂ̃u�����h�l�̌v�Z
    const Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };
    lastBlendWeight_ = nextBlendWeight;

    //���������S�ɓ��Ă͂܂��Ă���A�j���[�V�����͂Ȃ���
    for (auto& animData : blendAnims_)
    {
        if (nextBlendWeight.x != animData.weight_.x)continue;
        if (nextBlendWeight.y != animData.weight_.y)continue;

        model->Animate(animData.index_, timeStamp_, *animatedNodes_);

        return;
    }

    //Weight�l�����_�Ƃ��đ��`�l�ی��Ƀ��[�V�����f�[�^�𕪂���   
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            ���ی�              |           ���ی�
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            ��O�ی�              |           ��l�ی�
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    std::vector<BlendAnimData*> blendAnimDatas[4];

    for (auto& animData : blendAnims_)
    {
        //���݂̃u�����h�l���炱�̃A�j���[�V�����̃u�����h�l�������Ƃ��̑��Βl
        Vector2 relative = animData.weight_ - nextBlendWeight;

        //�e�ی��ɃA�j���[�V�����Ńf�[�^��ǉ�
        if (relative.x >= 0 && relative.y >= 0)
        {
            blendAnimDatas[0].emplace_back(&animData);
        }
        else if (relative.x < 0 && relative.y >= 0)
        {
            blendAnimDatas[1].emplace_back(&animData);
        }
        else if(relative.x < 0 && relative.y < 0)
        {
            blendAnimDatas[2].emplace_back(&animData);
        }
        else
        {
            blendAnimDatas[3].emplace_back(&animData);
        }
    }

    //�e�ی��ōł��߂��A�j���[�V�������擾����
    BlendAnimData* animData[4] = {};
    float distToNextWeight[4];
    int emptyCount = 0;//�v�f���O�̏ی��̐�
    //�v�f���O�̏ی����r�b�g���Z�œo�^�@�P�r�b�g�ڂ���ی��Ƃ��čl����
    for (int i = 0; i < 4; i++)
    {
        float nearestDist = FLT_MAX;

        if (blendAnimDatas[i].size() == 0)
        {
            emptyCount++;
            continue;
        }

        animData[i] = blendAnimDatas[i].at(0);

        for (auto& a : blendAnimDatas[i])
        {
            Vector2 relative = animData[i]->weight_ - nextBlendWeight;
            float dist = relative.Length();
            
            //�������r
            if (nearestDist > dist)
            {
                //�ŋߒl�Ɠo�^����Ă���A�j���[�V�������Đݒ�
                nearestDist = dist;
                animData[i] = a;
            }
        }

        //���̃A�j���[�V�����f�[�^����ŏI�I�ȃu�����h���܂ł̒�����ݒ�
        distToNextWeight[i] = nearestDist;
    }

    //�擾�����A�j���[�V�������u�����h���Ă���
    //�܂��f�[�^����A�j���[�V�������擾
    for (int i = 0; i < 4; i++)
    {
        if (animData[i]) 
        {
            model->Animate(animData[i]->index_, timeStamp_, blendAnimNodes_[i]);
        }
    }

    //�擾�����A�j���[�V�������g���ď��ԂɃu�����h���Ă���
    //���g����̏ی����X�L�b�v����
    int secondNodesNo = 0;//�u�����h���ꂽ�m�[�h������ϐ��̗v�f�ԍ�
    for (int i = 0; i < 4; i++)
    {
        if (!animData[i])continue;
        for (int j = i + 1; j < 4; j++)
        {
            if (!animData[j])continue;

            //�u�����h�����Z�o����
            //�����O�Ɍv�Z����animData[i]�̃u�����h���̋�����animData[j]�̃u�����h���̋����̔䗦���狁�߂Ă���
            float weight = distToNextWeight[j] / (distToNextWeight[i] + distToNextWeight[j]);
            model->BlendAnimations(blendAnimNodes_[i], blendAnimNodes_[j], weight, secondBlendAnimNodes_[secondNodesNo]);
            secondNodesNo++;
            i = j;
            goto skip;
        }

        //�u�����h���郂�[�V�����������Ȃ������ꍇ�͂��̂܂ܑ��
        secondBlendAnimNodes_[secondNodesNo] = blendAnimNodes_[i];             

        //�������胂�[�V�����u�����h����Ă���ꍇ�͏������΂�
    skip:
        continue;
    }

    //�u�����h�������ʁA�Q�̃��[�V�������o���Ă���͂��Ȃ̂ŁA����Ƀu�����h����
    //��������̏ی��̐����Q�ȏ゠��Ƃ��̓u�����h���ꂽ���ʂ͂P�����Ȃ��̂ł��̂܂܃��f���ɑ���
    switch (emptyCount)
    {
    case 0:
    {
        //�u�����h���̌v�Z
        float weight1 = distToNextWeight[0] + distToNextWeight[1];
        float weight2 = distToNextWeight[2] + distToNextWeight[3];
        float weight = weight2 / (weight1 + weight2);

        //����Ƀu�����h���A���f���֓n��
        model->BlendAnimations(secondBlendAnimNodes_[0], secondBlendAnimNodes_[1], weight, *animatedNodes_);


        break;
    }
    case 1:
    case 2:
    case 3:
        //2��3�̎��̏����͓���
        //���̂܂܂������u�����h�����A�������͂��̂܂܂̌��ʂ����f���֓n��
        

        break;
    }
#endif // 0
}

void AnimBlendSpace2D::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }
        ImGui::SliderFloat("Blend Weight X", &blendWeight_.x, minWeight_.x, maxWeight_.x);
        ImGui::SliderFloat("Blend Weight Y", &blendWeight_.y, minWeight_.y, maxWeight_.y);
        //ImGui::SliderFloat("Blend Weight X", &blendWeight_.x, 0.0f, 1.0f);
        //ImGui::SliderFloat("Blend Weight Y", &blendWeight_.y, 0.0f, 1.0f);
        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

void AnimBlendSpace2D::AddBlendAnimation(const int& index, const Vector2& weight)
{
    auto& a = blendAnimDatas_.emplace_back(BlendAnimData(index, weight));
}

//AbyssEngine::AnimBlendSpaceFlyMove::AnimBlendSpaceFlyMove(SkeletalMesh* model, const std::string& name, AnimBlendSpace2D* blendSpace2D, AnimBlendSpace1D* blendSpace1D)
//    : Animation(model,name)
//{
//
//}
