#include "AssetManager.h"
#include "Object.h"
#include <assert.h>

using namespace AbyssEngine;

void AssetManager::RegistrationAsset(std::shared_ptr<Object> asset)
{
    //アセットのIDをハッシュにしてキャッシュ
    cacheAsset_[asset->GetInstanceId()] = asset;
}

void AssetManager::EraseAsset(const std::string& id)
{
    const auto it = cacheAsset_.erase(id);
    assert(it != 0);
}

std::weak_ptr<Object> AssetManager::GetAssetFromId(const std::string& id)
{
    //IDからアセットを取得（多分つかわん？？）
    std::weak_ptr<Object>  asset;
    if (const auto it = cacheAsset_.find(id);it != cacheAsset_.end())
    {
        return it->second;
    }
    return asset;
}

void AssetManager::Exit()
{
    cacheTexture_.clear();
    cacheAsset_.clear();
}
