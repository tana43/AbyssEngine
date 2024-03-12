#include "Object.h"
#include "AssetManager.h"
#include "Engine.h"
#include "SystemFunction.h"

using namespace AbyssEngine;

Object::Object()
{
    instanceId_ = SystemFunction::CreateID();
}

Object::Object(const Object& obj)
{
    instanceId_ = SystemFunction::CreateID();
}

Object::~Object()
{
    Engine::assetManager_->EraseAsset(instanceId_);
}
