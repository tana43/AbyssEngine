#pragma once
#include <memory>
#include <vector>
#include "Component.h"

namespace AbyssEngine
{
    class Actor : public std::enable_shared_from_this<Actor>
    {
    public:
        Actor();
        ~Actor();

        std::shared_ptr<Transform> transform_;                  //�A�^�b�`����Ă���Transform
        std::vector<std::shared_ptr<Component>> componentList_; //�A�^�b�`����Ă���R���|�[�l���g�̃��X�g

    private:
        void Initialize();  //������
        void Release();     //��n��

    private:
        bool active_ = true;    //�A�N�e�B�u���
        bool activeOld_ = true; //�ύX���g���K�[�p�L���b�V��
    };
}

