#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    //�S�ẴI�u�W�F�N�g�̊��N���X
    class Object : public std::enable_shared_from_this<Object>
    {
    public:
        std::string name_;

    public:
        Object();
        Object(const Object& obj);
        virtual ~Object();

        [[nodiscard]] std::string ToString() const { return name_; }//���O��Ԃ�
        [[nodiscard]] std::string GetInstanceId() const { return instanceId_; }
        

    private:
        std::string instanceId_;//�C���X�^���X���ɐ�������郆�j�[�NID
    };
}

