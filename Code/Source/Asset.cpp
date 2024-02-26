#include "Asset.h"
#include "AssetManager.h"
#include "Engine.h"
#include "SystemFunction.h"

using namespace AbyssEngine;

Asset::Asset()
{
    instanceId_ = SystemFunction::CreateID();
}

Asset::Asset(const Asset& obj)
{
    instanceId_ = SystemFunction::CreateID();
}

Asset::~Asset()
{
    Engine::assetManager_->EraseAsset(instanceId_);
}
