#pragma once

#include <vector>
#include "State.h"
#include "imgui/imgui.h"

#include "Component.h"

namespace AbyssEngine
{
    template <class T>
    class StateMachine : public ScriptComponent
    {
    public:
        StateMachine() {};
        ~StateMachine();

        void UpdateBefore()override; // �X�V����

        void DrawImGui()override;               // ImGui�p

        void SetState(int newState);        // �X�e�[�g�Z�b�g
        void ChangeState(int newState);     // �X�e�[�g�ύX
        void RegisterState(T* state);       // �X�e�[�g�o�^

    public:// �擾�E�ݒ�
        int GetStateIndex();                // ���݂̃X�e�[�g�ԍ��擾
        T* GetCurrentState() { return currentState_; } // ���݂̃X�e�[�g�擾

    private:
        T* currentState_;            // ���݂̃X�e�[�g
        std::vector<T*> statePool_;  // �e�X�e�[�g��ێ�����z��
    };

    template<class T>
    inline StateMachine<T>::~StateMachine()
    {
        // �o�^�����X�e�[�g���폜����
        for (T* state : statePool_)
        {
            delete state;
        }
        statePool_.clear();
    }

    template<class T>
    inline void StateMachine<T>::UpdateBefore()
    {
        currentState_->Update();
    }

    //template<class T>
    //inline void StateMachine<T>::Update()
    //{
    //    currentState_->Update();
    //}

    template<class T>
    inline void StateMachine<T>::DrawImGui()
    {
        static int state = 0;
        if (ImGui::TreeNode("stateMachine"))
        {
            ImGui::Text(currentState_->GetName());

            float temp = currentState_->GetTimer();
            ImGui::DragFloat("time", &temp);

            ImGui::SliderInt("state", &state, 0, statePool_.size() - 1);
            if (ImGui::Button("Set State"))
            {
                ChangeState(state);
            }

            ImGui::TreePop();
        }
    }

    template<class T>
    inline void StateMachine<T>::SetState(int newState)
    {
        currentState_ = statePool_.at(newState);
        currentState_->Initialize();
    }

    template<class T>
    inline void StateMachine<T>::ChangeState(int newState)
    {
        currentState_->Finalize();
        SetState(newState);
    }

    template<class T>
    inline void StateMachine<T>::RegisterState(T* state)
    {
        statePool_.emplace_back(state);
    }

    template<class T>
    inline int StateMachine<T>::GetStateIndex()
    {
        int i = 0;
        for (T* state : statePool_)
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
