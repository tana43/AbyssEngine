#pragma once
#include "Component.h"
#include <string>

namespace AbyssEngine
{
    class Collider : public AbyssEngine::Component
    {
    public:
        Collider() {}
        ~Collider() {}

        //void Initialize(const std::shared_ptr<Actor>& actor);

        void SetFilePath(const char* path) { filePath_ = path; }

        void SetEnable(bool value) { enabled_ = value; }
        [[nodiscard]] bool GetEnabled() const { return enabled_; }

    public:


    protected:
        bool enabled_ = true;//”»’èˆ—‚·‚é‚©

        std::string filePath_;

    };
}
