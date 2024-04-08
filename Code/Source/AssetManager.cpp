#include "AssetManager.h"
#include "Asset.h"
#include <assert.h>

using namespace AbyssEngine;

void AssetManager::RegistrationAsset(std::shared_ptr<Asset> asset)
{
    //�A�Z�b�g��ID���n�b�V���ɂ��ăL���b�V��
    const auto i = asset->GetInstanceId();
    cacheAsset_[asset->GetInstanceId()] = asset;
}

void AssetManager::EraseAsset(const std::string& id)
{
    const auto it = cacheAsset_.erase(id);
    assert(it != 0);
}

std::weak_ptr<Asset> AssetManager::GetAssetFromId(const std::string& id)
{
    //ID����A�Z�b�g���擾�i���������H�H�j
    std::weak_ptr<Asset>  asset;
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
