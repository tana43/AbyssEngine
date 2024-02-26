#pragma once
#include <memory>

namespace AbyssEngine
{
    class Scene;

    class SceneManager
    {
    public:
        SceneManager();

        std::unique_ptr<Scene>& GetActiveScene(); //���݃A�N�e�B�u�ȃV�[�����擾

        bool run = false;   //�Đ���
        bool pose = false;  //�|�[�Y��

    private:
        void Exit() const;
        void Update(); //�X�V

        std::unique_ptr<Scene> sctiveScene_; //���݃A�N�e�B�u�ȃV�[��

    };

}

