#pragma once
#include "Component.h"

namespace AbyssEngine
{
    class Collider : public Component
    {
    public:
        void Initialize(const std::shared_ptr<Actor>& actor);

    private:

    };
}
