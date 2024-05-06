#pragma once
#include <memory>
#include <unordered_map>
#include "DXSystem.h"
#include <string>

namespace AbyssEngine
{
    class Asset;
    class Texture;


    //�A�Z�b�g���Ǘ�����N���X
    class AssetManager
    {
    public:
        void RegistrationAsset(std::shared_ptr<Asset> Asset);
        void EraseAsset(const std::string& id);
        std::weak_ptr<Asset> GetAssetFromId(const std::string& id);
        
        std::unordered_map<std::string, std::shared_ptr<Texture>> cacheTexture_{}; //�e�N�X�`���̃L���b�V��


        void Exit();//��n��

    private:
        std::unordered_map<std::string, std::weak_ptr<Asset>> cacheAsset_{};
   
    };
}


