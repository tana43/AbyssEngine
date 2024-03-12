#pragma once
#include <memory>
#include <unordered_map>
#include "DXSystem.h"
#include <string>

namespace AbyssEngine
{
    class Object;
    class Texture;


    //アセットを管理するクラス
    class AssetManager
    {
    public:
        void RegistrationAsset(std::shared_ptr<Object> asset);
        void EraseAsset(const std::string& id);
        std::weak_ptr<Object> GetAssetFromId(const std::string& id);
        
        std::unordered_map<std::string, std::shared_ptr<Texture>> cacheTexture_{}; //テクスチャのキャッシュ

        void Exit();//後始末

    private:
        std::unordered_map<std::string, std::weak_ptr<Object>> cacheAsset_{};
   
    };
}


