#pragma once
#include "System.h"
#include "Transform.h"
#include "Light.h"

class LightSystem : public System<Transform, Light>
{
public:
    void Render();
};
