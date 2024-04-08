#pragma once
#include <memory>
#include <string>

namespace AbyssEngine
{
    //�S�ẴI�u�W�F�N�g�̊��N���X
    class Asset : public std::enable_shared_from_this<Asset>
    {
    public:
        std::string name_;

    public:
        Asset();
        Asset(const Asset& obj);
        virtual ~Asset();

        [[nodiscard]] std::string ToString() const { return name_; }//���O��Ԃ�
        [[nodiscard]] std::string GetInstanceId() const { return instanceId_; }
        

    private:
        std::string instanceId_;//�C���X�^���X���ɐ�������郆�j�[�NID
    };
}

