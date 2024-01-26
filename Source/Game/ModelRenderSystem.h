#pragma once
#include "System.h"
#include "Transform.h"
#include "ModelCollection.h"

// TODO: remove
#include "Model.h"

class ModelRenderSystem : public System<ModelCollection, Transform>
{
public:
    void Render();
private:
    //bool ShouldRender() const;
};
