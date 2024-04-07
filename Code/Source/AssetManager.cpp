#include "AssetManager.h"
#include "Object.h"
#include <assert.h>

using namespace AbyssEngine;

void AssetManager::RegistrationAsset(std::shared_ptr<Object> asset)
{
    //�A�Z�b�g��ID���n�b�V���ɂ��ăL���b�V��
    cacheAsset_[asset->GetInstanceId()] = asset;
}

void AssetManager::EraseAsset(const std::string& id)
{
    const auto it = cacheAsset_.erase(id);
    assert(it != 0);
}

std::weak_ptr<Object> AssetManager::GetAssetFromId(const std::string& id)
{
    //ID����A�Z�b�g���擾�i���������H�H�j
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
