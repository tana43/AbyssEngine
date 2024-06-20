#pragma once
#include <memory>
#include <unordered_map>
#include "DXSystem.h"
#include <string>

namespace AbyssEngine
{
    class Asset;
    class Texture;
    class GltfSkeletalMesh;
    class GltfStaticMesh;
    class GltfCollisionMesh;

    //アセットを管理するクラス
    class AssetManager
    {
    public:
        void RegistrationAsset(std::shared_ptr<Asset> Asset);
        void EraseAsset(const std::string& id);
        std::weak_ptr<Asset> GetAssetFromId(const std::string& id);
        
        std::unordered_map<std::string, std::shared_ptr<Texture>> cacheTexture_{}; //テクスチャのキャッシュ
        std::unordered_map<std::string, std::shared_ptr<GltfSkeletalMesh>> cacheSkeletalMesh_{}; //スケルタルメッシュのキャッシュ
        std::unordered_map<std::string, std::shared_ptr<GltfStaticMesh>> cacheStaticMesh_{}; //スタティックメッシュのキャッシュ
        std::unordered_map<std::string, std::shared_ptr<GltfCollisionMesh>> cacheCollisionMesh_{}; //コリジョンメッシュのキャッシュ


        void Exit();//後始末

    private:
        std::unordered_map<std::string, std::weak_ptr<Asset>> cacheAsset_{};
   
    };
}


