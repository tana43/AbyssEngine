#include "AssetManager.h"
#include "Asset.h"
#include <assert.h>

using namespace AbyssEngine;

void AssetManager::RegistrationAsset(std::shared_ptr<Asset> Asset)
{
    //�A�Z�b�g��ID���n�b�V���ɂ��ăL���b�V��
    const auto i = Asset->GetInstanceId();
    cacheAsset_[Asset->GetInstanceId()] = Asset;
}

void AssetManager::EraseAsset(const std::string& id)
{
    const auto it = cacheAsset_.erase(id);
    assert(it != 0);
}

std::weak_ptr<Asset> AssetManager::GetAssetFromId(const std::string& id)
{
    //ID����A�Z�b�g���擾�i���������H�H�j
    std::weak_ptr<Asset>  Asset;
    if (const auto it = cacheAsset_.find(id);it != cacheAsset_.end())
    {
        return it->second;
    }
    return Asset;
}

void AssetManager::Exit()
{
    cacheTexture_.clear();
    cacheSkeletalMesh_.clear();
    cacheStaticMesh_.clear();
    cacheAsset_.clear();
}
