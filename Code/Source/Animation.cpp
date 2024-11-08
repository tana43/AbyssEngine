#include "Animation.h"
#include "SkeletalMesh.h"
#include "Engine.h"
#include "Animator.h"
#include "Actor.h"
#include "Transform.h"

#include "RenderManager.h"
#include "DebugRenderer.h"

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

std::vector<GeometricSubstance::Node> Animation::UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished)
{
    UpdateTime();

    bool flag = model->Animate(animIndex_, timeStamp_, animatedNodes_, loopFlag_);
    if (animationFinished)
    {
        *animationFinished = flag;
    }

    //タイマーのループ処理
    if (!flag)
    {
        if (animIndex_ < model->animations_.size())
        {
            if (model->animations_.at(animIndex_).duration_ < timeStamp_)
            {
                timeStamp_ = 0;
            }
        }
    }

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
        ImGui::Checkbox("RootMotion", &rootMotion_);
        ImGui::DragFloat("RootMotion Speed", &rootMotionSpeed_, 0.01f, 0.1f);

        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

void Animation::UpdateTime()
{
    timeStamp_ += animSpeed_ * animator_->GetActor()->GetDeltaTime();
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

//現在のブレンドの重みから正に近いモーションと、負に近いモーションの二つを取得し、ブレンドする
std::vector<GeometricSubstance::Node> AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model,bool* animationFinished)
{
    //ブレンドスペースは必ずループ再生なのでアニメーションの再生は終わらない
    if (animationFinished)
    {
        *animationFinished = false;
    }

    UpdateTime();

    //ブレンドの速度制限
    const float blendMaxSpeed = 2.0f * animator_->GetActor()->GetDeltaTime();
    const float blendSpeed = std::clamp(blendWeight_ - lastBlendWeight_, -blendMaxSpeed, blendMaxSpeed);

    //実際のブレンド値の計算
    const float nextBlendWeight = std::clamp(lastBlendWeight_ + blendSpeed, minWeight_, maxWeight_);

    //ブレンド率が同じアニメーションがあるか
    for (auto& anim : blendAnimDatas_)
    {
        if (anim.weight_ == nextBlendWeight)
        {
            model->Animate(anim.index_, timeStamp_, animatedNodes_);
            lastBlendWeight_ = nextBlendWeight;
            return animatedNodes_;
        }
    }

    //blendAnims[1]の方が[0]よりもweight値が大きい
    BlendAnimData blendAnims[2] = {blendAnimDatas_[0],blendAnimDatas_[0]};
    for (auto& animData : blendAnimDatas_)
    {
        //最もweight値が小さいデータを代入
        if (blendAnims[0].weight_ >= animData.weight_)
        {
            blendAnims[0] = animData;
        }
        //最もweight値が大きいデータを代入
        else if (blendAnims[1].weight_ < animData.weight_)
        {
            blendAnims[1] = animData;
        }
    }

    //ブレンドする２つのモーションを取る blendWeightが[0]と[1]のweight値に収まる範囲のアニメーションを探す
    if (blendAnimDatas_.size() >= 2)
    {
        for (int i = 0; i < blendAnimDatas_.size(); i++)
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
    
    //二つのモーションデータから実際に補完率に使われる値を計算
    //急なブレンドの重さの変化を防ぐように前回の値から変動する値に上限を設ける
    const float maxBlendWeight = blendAnims[1].weight_ - blendAnims[0].weight_;
    const float blendWeight = std::clamp((nextBlendWeight - blendAnims[0].weight_) / maxBlendWeight,0.0f,1.0f);

    //モーションブレンド
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
    //引数から渡される初期モーションのみ登録
    blendAnimDatas_.emplace_back(BlendAnimData(index,weight));

    for(auto& animNode : blendAnimNodes_)
    {
        animNode = animatedNodes_;
    }
    secondBlendAnimNodes_ = animatedNodes_;
}

std::vector<GeometricSubstance::Node> AnimBlendSpace2D::UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished)
{
    //ブレンドスペースは必ずループ再生なのでアニメーションの再生は終わらない
    if (animationFinished)
    {
        *animationFinished = false;
    }
    
    UpdateTime();

    //ブレンドの速度制限
    const float blendMaxSpeed = 5.0f * animator_->GetActor()->GetDeltaTime();
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //ブレンド値の算出
    Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };
    
    //中央からの距離が１以上離れている場合は、制限を掛けておく
    //weightLengthはブレンド率として最後の方で使う
    float weightLength = nextBlendWeight.Length();
    if (weightLength > 1.0f)
    {
        //値を正規化し長さを１に抑える
        nextBlendWeight.Normalize();
        weightLength = 1.0f;
    }
    lastBlendWeight_ = nextBlendWeight;

    //自分のブレンド値がどこに位置するかを判定し、中央に位置する待機モーションと両隣の２つの移動モーションとを
    // ブレンド、最大二回のブレンドから最終的なモーションを算出する
    //イメージ
    //  * : 各アニメーション
    //  + : ブレンド値
    //                           |
    //                           *(0,1)
    //                           |      (0.7,0.5) 左と下のモーションをブレンドし、さらにそれと中央の待機モーションをブレンド
    //                           |      +
    //               (-1,0)      |          (1,0)
    //----------------*----------*----------*------------------
    //                           |
    //                           |
    //                           |
    //                           *(0,-1)
    //                           |

    //まず両隣のモーションを取得する
    //時計回りになるような順番で要素を入れる
    BlendAnimData animDatas[2];
    BlendSituation blendSituation;
    {

        //どの程度ブレンドをするべき状態かも判定する
        if (nextBlendWeight.x == 0 && nextBlendWeight.y == 0)
        {
            blendSituation = BlendSituation::None;
        }
        else if (nextBlendWeight.x == 0)
        {
            blendSituation = BlendSituation::Once;
            //左右はないので前後どちらか
            if (nextBlendWeight.y > 0)animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_F)];
            else animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_B)];
        }
        else if (nextBlendWeight.y == 0)
        {
            blendSituation = BlendSituation::Once;
            //前後はないので左右どちらか
            if (nextBlendWeight.x > 0)animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_R)];
            else animDatas[0] = blendAnimDatas_[static_cast<int>(State::Move_L)];
        }
        else//これ以降は必ず２回ブレンドする必要がある場合になる
        {
            blendSituation = BlendSituation::Twice;

            //x>0,y>0から時計回りに判定していく
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

    //実際にモーションをブレンドしていく
    switch (blendSituation)
    {
    case BlendSituation::None:
        //何もしなくていいので待機モーションを再生
        model->Animate(
            blendAnimDatas_[static_cast<int>(State::Idle)].index_,
            timeStamp_,
            animatedNodes_);
        break;

    case BlendSituation::Once:
        //BlendSpace1Dと同じモーションブレンド
        model->Animate(blendAnimDatas_[static_cast<int>(State::Idle)].index_,timeStamp_, blendAnimNodes_[0]);
        model->Animate(animDatas[0].index_, timeStamp_, blendAnimNodes_[1]);
        model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], weightLength, animatedNodes_);
        break;

    case BlendSituation::Twice:
        //斜め移動モーション

        //１回目のブレンド値の算出
        //2つのモーションデータから見た本来のブレンド値までの距離の比を使う
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

    //没
#if 0
    //ブレンドの速度制限
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //実際のブレンド値の計算
    const Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };
    lastBlendWeight_ = nextBlendWeight;

    //条件が完全に当てはまっているアニメーションはないか
    for (auto& animData : blendAnims_)
    {
        if (nextBlendWeight.x != animData.weight_.x)continue;
        if (nextBlendWeight.y != animData.weight_.y)continue;

        model->Animate(animData.index_, timeStamp_, *animatedNodes_);

        return;
    }

    //Weight値を原点として第一～四象限にモーションデータを分ける   
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            第二象限              |           第一象限
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            第三象限              |           第四象限
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    std::vector<BlendAnimData*> blendAnimDatas[4];

    for (auto& animData : blendAnims_)
    {
        //現在のブレンド値からこのアニメーションのブレンド値を見たときの相対値
        Vector2 relative = animData.weight_ - nextBlendWeight;

        //各象限にアニメーションでデータを追加
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

    //各象限で最も近いアニメーションを取得する
    BlendAnimData* animData[4] = {};
    float distToNextWeight[4];
    int emptyCount = 0;//要素が０の象限の数
    //要素数０の象限をビット演算で登録　１ビット目を第一象限として考える
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
            
            //距離を比較
            if (nearestDist > dist)
            {
                //最近値と登録されているアニメーションを再設定
                nearestDist = dist;
                animData[i] = a;
            }
        }

        //このアニメーションデータから最終的なブレンド率までの長さを設定
        distToNextWeight[i] = nearestDist;
    }

    //取得したアニメーションをブレンドしていく
    //まずデータからアニメーションを取得
    for (int i = 0; i < 4; i++)
    {
        if (animData[i]) 
        {
            model->Animate(animData[i]->index_, timeStamp_, blendAnimNodes_[i]);
        }
    }

    //取得したアニメーションを使って順番にブレンドしていく
    //中身が空の象限をスキップする
    int secondNodesNo = 0;//ブレンドされたノードを入れる変数の要素番号
    for (int i = 0; i < 4; i++)
    {
        if (!animData[i])continue;
        for (int j = i + 1; j < 4; j++)
        {
            if (!animData[j])continue;

            //ブレンド率を算出する
            //少し前に計算したanimData[i]のブレンド率の距離とanimData[j]のブレンド率の距離の比率から求めていく
            float weight = distToNextWeight[j] / (distToNextWeight[i] + distToNextWeight[j]);
            model->BlendAnimations(blendAnimNodes_[i], blendAnimNodes_[j], weight, secondBlendAnimNodes_[secondNodesNo]);
            secondNodesNo++;
            i = j;
            goto skip;
        }

        //ブレンドするモーションが何もなかった場合はそのまま代入
        secondBlendAnimNodes_[secondNodesNo] = blendAnimNodes_[i];             

        //しっかりモーションブレンドされている場合は処理を飛ばす
    skip:
        continue;
    }

    //ブレンドした結果、２つのモーションが出来ているはずなので、さらにブレンドする
    //ただし空の象限の数が２つ以上あるときはブレンドされた結果は１つしかないのでそのままモデルに送る
    switch (emptyCount)
    {
    case 0:
    {
        //ブレンド率の計算
        float weight1 = distToNextWeight[0] + distToNextWeight[1];
        float weight2 = distToNextWeight[2] + distToNextWeight[3];
        float weight = weight2 / (weight1 + weight2);

        //さらにブレンドし、モデルへ渡す
        model->BlendAnimations(secondBlendAnimNodes_[0], secondBlendAnimNodes_[1], weight, *animatedNodes_);


        break;
    }
    case 1:
    case 2:
    case 3:
        //2と3の時の処理は同じ
        //そのままさっきブレンドした、もしくはそのままの結果をモデルへ渡す
        

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

AbyssEngine::AnimBlendSpaceFlyMove::AnimBlendSpaceFlyMove(SkeletalMesh* model, const std::string& name_, AnimBlendSpace2D* blendSpace2D, AnimBlendSpace1D* blendSpace1D)
    : Animation(model,name_,0/*なんでもいい*/),
    blendSpace1D_(blendSpace1D),
    blendSpace2D_(blendSpace2D)
{

}

std::vector<GeometricSubstance::Node> AbyssEngine::AnimBlendSpaceFlyMove::UpdateAnimation(GltfSkeletalMesh* model,bool* animationFinished)
{
    if (animationFinished)
    {
        *animationFinished = false;
    }

    //各モーションを計算
    const auto& blendNode1D = blendSpace1D_->UpdateAnimation(model);
    const auto& blendNode2D = blendSpace2D_->UpdateAnimation(model);

    //ブレンドスペース１Dは上下移動、２Dは水平移動
    //この関係から動いた方向を算出し、ブレンド率を求める
    float weight;

    //移動方向のベクトルが０付近ならブレンド率を０へ
    if (moveVec_.LengthSquared() < 0.01f)
    {
        weight = 0.0f;
    }
    else
    {
        Vector3 f = { moveVec_.x,0,moveVec_.z };

        f.Normalize();
        moveVec_.Normalize();

        const float dot = f.Dot(moveVec_);
        weight = fabsf(dot);//ブレンド率
    }

    //なめらかにブレンドするために補完
    const float blendMaxSpeed = 2.0f * animator_->GetActor()->GetDeltaTime();
    const float blendSpeed = std::clamp(weight - lastBlendWeight_, -blendMaxSpeed, blendMaxSpeed);
    const float nextBlendWeight = std::clamp(lastBlendWeight_ + blendSpeed, 0.0f, 1.0f);
    lastBlendWeight_ = nextBlendWeight;

    //モーションブレンド
    model->BlendAnimations(blendNode1D, blendNode2D, nextBlendWeight, animatedNodes_);

    return animatedNodes_;
}

AnimAimIK::AnimAimIK(SkeletalMesh* model, const std::string& name_) : Animation(model, name_, 0/*なんでもいい*/)
{

}

void AbyssEngine::AnimAimIK::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }

        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);

        ImGui::DragFloat3("Target Position", &targetPosition_.x, 0.05f);
        ImGui::SliderFloat("Arm Extension", &armExtension, 0.0f, 3.0f);
        ImGui::DragFloat3("Pole Local Position", &poleLocalPosition_.x, 0.01f);

        //無視するノードが中間ノードより上にいているか
        ImGui::Checkbox("IgnoreNode is Mid Up",&isUpIgnoreNode_);

        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }

#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(targetPosition_, 1.0f, Vector4(0, 1, 0, 1));
#endif
}

std::vector<GeometricSubstance::Node> AbyssEngine::AnimAimIK::UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished)
{
    //ベースのモーションを取得
    //model->Animate(baseAnimationIndex_, timeStamp_,animatedNodes_);

    const auto& skeletalMesh = animator_->GetSkeletalMesh().lock();
    const Matrix& worldMatrix = animator_->GetTransform()->GetWorldMatrix();

    //各ノードを取得
    GeometricSubstance::Node& rootNode = model->GetNode(animatedNodes_,rootNodeName_);
    GeometricSubstance::Node& midNode  = model->GetNode(animatedNodes_,midNodeName_);
    GeometricSubstance::Node& tipNode  = model->GetNode(animatedNodes_,tipNodeName_);


    //無視する必要のあるノードを取得
    GeometricSubstance::Node* ignoreNode = nullptr;
    if (!ignoreNodeName_.empty())
    {
        ignoreNode = &model->GetNode(animatedNodes_, ignoreNodeName_);
    }
    GeometricSubstance::Node* ignoreNodeSecond = nullptr;
    if (!ignoreNodeNameSecond_.empty())
    {
        ignoreNodeSecond = &model->GetNode(animatedNodes_, ignoreNodeNameSecond_);
    }
    rootNode.rotation_ = { 0,0,0,1 };
    midNode.rotation_ = { 0,0,0,1 };
    tipNode.rotation_ = { 0,0,0,1 };
    if (ignoreNode)ignoreNode->rotation_ = { 0,0,0,1 };
    if (ignoreNodeSecond)ignoreNodeSecond->rotation_ = { 0,0,0,1 };

    animatedNodes_[tipNode.children_[0]].rotation_ = { 0,0,0,1 };
    //model->NodeCumulateTransforms(animatedNodes_,rootNode);
    model->CumulateTransforms(animatedNodes_, 0);

    //ポールターゲット更新
    Matrix poleLocalTransform = DirectX::XMMatrixTranslation(poleLocalPosition_.x, poleLocalPosition_.y, poleLocalPosition_.z);
    //Matrix poleWorldTransform = poleLocalTransform * (midNode.globalTransform_ * worldMatrix);
    Matrix poleWorldTransform = poleLocalTransform * (rootNode.globalTransform_ * worldMatrix);

#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(DirectX::XMLoadFloat4x4(&poleWorldTransform).r[3], 1.0f, Vector4(0.2f, 1.0f, 0.0f, 1.0f));
#endif
    /*Matrix poleWorldTransform = (midNode.globalTransform_ * worldMatrix);
    Vector3 polePosition = DirectX::XMLoadFloat4x4(&poleWorldTransform).r[3];
    polePosition = polePosition + poleLocalPosition_;
    Engine::renderManager_->debugRenderer_->DrawSphere(polePosition, 1.0f, Vector4(0.2f, 1.0f, 0.0f, 1.0f));*/

    
    //AimIK
    {
        DirectX::XMMATRIX rootWorldTransform = rootNode.globalTransform_ * worldMatrix;
        DirectX::XMMATRIX midWorldTransform = midNode.globalTransform_ * worldMatrix;
        DirectX::XMMATRIX tipWorldTransform = tipNode.globalTransform_ * worldMatrix;
        Vector3 rootPos = rootWorldTransform.r[3];
        Vector3 midPos = midWorldTransform.r[3];
        Vector3 tipPos = tipWorldTransform.r[3];
      
        Vector3 rootToMid = midPos - rootPos;
        Vector3 midToTip = tipPos - midPos;


        //根元から先端までの距離
        float lengthRootToTarget;
        float lengthRootToMid;
        float lengthMidToTip;

        //間に回転を考慮しないノードが存在している場合は、そのノードの長さを考慮しておく
        if (ignoreNodeSecond)
        {
            const DirectX::XMMATRIX IgnoreWorldTransform = ignoreNode->globalTransform_ * worldMatrix;
            const Vector3 ignorePos = IgnoreWorldTransform.r[3];
            const Vector3 ignoreToMid = midPos - ignorePos;
            const DirectX::XMMATRIX IgnoreSecWorldTransform = ignoreNodeSecond->globalTransform_ * worldMatrix;
            const Vector3 ignoreSecPos = IgnoreSecWorldTransform.r[3];
            const Vector3 rootToIgnoreSec = ignoreSecPos - rootPos;

            //無視するノードが中間ノードよりも上か下か
            if (isUpIgnoreNode_)
            {
                //ルート->無視ノード２->無視ノード->中間->先端
                const Vector3 ignoreToIgnoreSec = ignoreSecPos - ignorePos;
                lengthRootToMid = rootToIgnoreSec.Length() + ignoreToIgnoreSec.Length() + ignoreToMid.Length();
                lengthMidToTip = midToTip.Length();
            }
            else
            {
                //ルート->無視ノード2->中間->無視ノード->先端
                const Vector3 ignoreSecToMid = midPos - ignoreSecPos;
                const Vector3 ignoreToTip = midPos - ignorePos;
                //lengthRootToMid = rootToMid.Length();
                lengthRootToMid = rootToIgnoreSec.Length() + ignoreSecToMid.Length();
                lengthMidToTip = ignoreToTip.Length() + ignoreToMid.Length();
            }
            
        }
        else if (ignoreNode)
        {
            const DirectX::XMMATRIX IgnoreWorldTransform = ignoreNode->globalTransform_ * worldMatrix;
            const Vector3 ignorePos = IgnoreWorldTransform.r[3];
            const Vector3 ignoreToMid = midPos - ignorePos;

            //無視するノードが中間ノードよりも上か下か
            if (isUpIgnoreNode_)
            {
                //ルート->無視ノード->中間->先端
                const Vector3 ignoreToRoot = rootPos - ignorePos;
                lengthRootToMid = ignoreToRoot.Length() + ignoreToMid.Length();
                lengthMidToTip = midToTip.Length();
            }
            else
            {
                //ルート->中間->無視ノード->先端
                const Vector3 ignoreToTip = midPos - ignorePos;
                lengthRootToMid = rootToMid.Length();
                lengthMidToTip = ignoreToTip.Length() + ignoreToMid.Length();
            }
        }
        else
        {
            lengthRootToMid = rootToMid.Length();
            lengthMidToTip = midToTip.Length();
        }

        const float armLength = lengthRootToMid + lengthMidToTip;

        // 根本ノードからターゲットまでのベクトルと腕を伸ばす量から
        // 実際のIKに使用するターゲットの座標を取得
        const Vector3 direRootToTarget = DirectX::XMVector3Normalize(targetPosition_ - rootPos);
        const Vector3 targetWorldPos = rootPos + (direRootToTarget * (armExtension * armLength));

#if _DEBUG
        Engine::renderManager_->debugRenderer_->DrawSphere(targetWorldPos,1.0f,Vector4(0,1.0f,0.5f,1.0f));
#endif // _DEBUG


        Vector3 rootToTarget = targetWorldPos - rootPos;
        Vector3 midToTarget = targetWorldPos - midPos;
        Vector3 dirRootToMid = DirectX::XMVector3Normalize(rootToMid);
        Vector3 dirRootToTarget = DirectX::XMVector3Normalize(rootToTarget);

        lengthRootToTarget = rootToTarget.Length();

        //ターゲットの方へボーンを回転
        const Vector3 palePos = DirectX::XMLoadFloat4x4(&poleWorldTransform).r[3];
        Vector3 rootToPale = rootPos - palePos;
        Vector3 dirRootToPale;
        rootToPale.Normalize(dirRootToPale);

        float angle = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dirRootToMid, dirRootToTarget));
        // ※acosf関数は-1.0未満、+1.0超過の値を入れるとエラーの値(nan)を返すので修正しておく
        angle = ::acosf(std::clamp(angle, -1.0f, +1.0f));
        animator_->RotateBone(animatedNodes_, rootNode, dirRootToMid, dirRootToTarget);
        //animator_->RotateBone(animatedNodes_, rootNode, dirRootToPale, dirRootToTarget,angle);
        //model->CumulateTransforms(animatedNodes_, 0);
        //return animatedNodes_;
        //根元からターゲットまでの距離
        float dist = rootToTarget.Length();

        //ボーンの向きがまっすぐにならない
        if (dist < armLength)
        {
            using namespace DirectX;

            //三角形の面積を求める
            float a = lengthRootToMid; //斜辺
            float b = lengthRootToTarget; // 底辺
            float c = lengthMidToTip;

            float s = (a + b + c) / 2.0f;
            float S = sqrtf(s * (s - a) * (s - b) * (s - c));


            //三角形の高さを求める
            float triHeight = S * 2.0f / a;

            //直角三角形の斜辺と高さから角度を求める
            float angle = asinf(triHeight / b);

            if (angle != 0.0f)
            {
                //ポールターゲットから回転軸を算出
                const Vector3 palePos = DirectX::XMLoadFloat4x4(&poleWorldTransform).r[3];
                Vector3 rootToPale = palePos - rootPos;
                rootToPale.Normalize();

                animator_->RotateBone(animatedNodes_,rootNode, dirRootToMid, rootToPale, angle);
            }
        }

        //根本ノード以下のワールド行列を更新
        //model->NodeCumulateTransforms(animatedNodes_, rootNode);
        model->CumulateTransforms(animatedNodes_, 0);

        //中間ボーンをターゲットの方へ向くように
        {
            midWorldTransform = midNode.globalTransform_ * worldMatrix;
            tipWorldTransform = tipNode.globalTransform_ * worldMatrix;
            midPos = midWorldTransform.r[3];
            tipPos = tipWorldTransform.r[3];
            Vector3 midToTip = tipPos - midPos;
            Vector3 midToTarget = targetWorldPos - midPos;

            // 中央ボーンをターゲット方向に向ける
            const Vector3 dirMidToTip = DirectX::XMVector3Normalize(midToTip);
            const Vector3 dirMidToTarget = DirectX::XMVector3Normalize(targetWorldPos - midPos);
            animator_->RotateBone(animatedNodes_, midNode, dirMidToTip, dirMidToTarget);

            // 中央ボーン以下のワールド行列更新
            //model->NodeCumulateTransforms(animatedNodes_, midNode);
            model->CumulateTransforms(animatedNodes_, 0);
        }

    }

    model->CumulateTransforms(animatedNodes_, 0);

    return animatedNodes_;
}

//GeometricSubstance::Node* AbyssEngine::AnimAimIK::CheckDirectLineBones(GltfSkeletalMesh* model)
//{
//    GeometricSubstance::Node& rootNode = model->GetNode(animatedNodes_, rootNodeName_);
//    GeometricSubstance::Node& midNode = model->GetNode(animatedNodes_, midNodeName_);
//    GeometricSubstance::Node& tipNode = model->GetNode(animatedNodes_, tipNodeName_);
//
//    //先端ノードから根本ノードまでにかけて親ノードが一致しない物があるか
//    if (tipNode.parent_ != midNode.index_)
//    {
//        return &animatedNodes_[tipNode.parent_];
//    }
//    else if ()
//    {
//
//    }
//
//    // TODO: return ステートメントをここに挿入します
//}

AbyssEngine::AnimAiming::AnimAiming(SkeletalMesh* model, const std::string& name_, const std::shared_ptr<AnimBlendSpace2D>& blendSpace2D, const std::shared_ptr<AnimAimIK>& rightHand, const std::shared_ptr<AnimAimIK>& leftHand)
    : Animation(model, name_, 0/*なんでもいい*/),
    blendSpace2d_(blendSpace2D),aimIkRight_(rightHand),aimIkLeft_(leftHand)
{

}


std::vector<GeometricSubstance::Node> AbyssEngine::AnimAiming::UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished)
{
    //各モーションを計算
    auto nodes = blendSpace2d_->UpdateAnimation(model);
    aimIkRight_->animatedNodes_ = nodes;
    nodes = aimIkRight_->UpdateAnimation(model);
    aimIkLeft_->animatedNodes_ = nodes;
    nodes = aimIkLeft_->UpdateAnimation(model);

    //ステートマシンでターゲットや移動方向、速度の設定

    return nodes;
}

void AbyssEngine::AnimAiming::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        aimIkLeft_->DrawImGui(animator);
        aimIkRight_->DrawImGui(animator);
    }
}
