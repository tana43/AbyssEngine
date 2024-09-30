#pragma once

#include "ScriptComponent.h"
#include "ComboAttack.h"
#include "Animation.h"

#include "imgui/imgui.h"

#include <vector>
#include <string>

namespace AbyssEngine
{
    template <class T>
    class ComboSystem : public ScriptComponent
    {
    public:
        ComboSystem() {};
        ~ComboSystem();

        void UpdateBefore()override; // �X�V����

        void DrawImGui()override;               // ImGui�p

        void SetCombo(int newCombo);     // �R���{�ύX
        void ChangeCombo(int newCombo);     // �R���{�ύX
        void RegisterCombo(T* combo);       // �R���{�o�^

    public:// �擾�E�ݒ�
        int GetCurrentComboIndex();                // ���݂̃X�e�[�g�ԍ��擾
        T* GetCurrentCombo() { return currentState_; } // ���݂̃X�e�[�g�擾

    private:
        T* currentCombo_;            // ���݂̃X�e�[�g
        std::vector<T*> comboList_;  // �e�X�e�[�g��ێ�����z��
    };

    template<class T>
    inline ComboSystem<T>::~ComboSystem()
    {
        // �o�^�����X�e�[�g���폜����
        for (T* combo : comboList_)
        {
            delete combo;
        }
        comboList_.clear();
    }

    template<class T>
    inline void ComboSystem<T>::UpdateBefore()
    {
        currentCombo_->Update();
    }

    //template<class T>
    //inline void StateMachine<T>::Update()
    //{
    //    currentState_->Update();
    //}

    template<class T>
    inline void ComboSystem<T>::DrawImGui()
    {
        if (ImGui::TreeNode("ComboSystem"))
        {
            ImGui::Text(currentCombo_->GetName());

            float temp = currentCombo_->GetTimer();
            ImGui::DragFloat("time", &temp);

            //��������R���{��ύX�ł���悤��
            static int index = 0;
            ImGui::SliderInt("comboIndex", &index, 0, comboList_.size() - 1);
            std::string setComboName_ = comboList_.at(index)->name_;
            if (ImGui::Button("Set Combo"))
            {
                ChangeCombo(index);
            }

            //�e�R���{�̃p�����[�^���������悤��
            ImGui::Text("------Combo Datas------");

            for (T* combo : comboList_)
            {
                if (ImGui::TreeNode(combo->GetName()))
                {
                    if (const auto& anim = combo->animation_.lock())
                    {
                        anim->DrawImGui();
                    }

                    ImGui::DragFloat("Attack",&combo->attack_,0.1f);
                    ImGui::DragFloat("Atk Start Time",&combo->attackTime_[0], 0.1f);
                    ImGui::DragFloat("Atk End Time",&combo->attackTime_[1], 0.1f);
                    ImGui::DragFloat("Input Buffering Time",&combo->inputBufferingTime_,0.01f);
                    ImGui::DragFloat("Cancel Time",&combo->cancelTime_,0.01f);
                }
            }

            ImGui::TreePop();
        }
    }

    template<class T>
    inline void ComboSystem<T>::SetCombo(int newCombo)
    {
        currentCombo_ = comboList_.at(newCombo);
        currentCombo_->PlayAnimation();
        currentCombo_->Begin();
    }

    template<class T>
    inline void ComboSystem<T>::ChangeCombo(int newCombo)
    {
        currentCombo_->Finalize();
        SetCombo(newCombo);
    }

    template<class T>
    inline void ComboSystem<T>::RegisterCombo(T* combo)
    {
        comboList_.emplace_back(combo);
    }

    template<class T>
    inline int ComboSystem<T>::GetCurrentComboIndex()
    {
        int i = 0;
        for (T* state : comboList_)
        {
            if (state == currentState_)
            {
                // i�ԍ��ڂ̃X�e�[�g�����^�[��
                return i;
            }
            ++i;
        }

        // �X�e�[�g��������Ȃ������Ƃ�
        return -1;
    }
}