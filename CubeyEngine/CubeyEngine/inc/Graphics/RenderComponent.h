#pragma once

#include "Core\Component.h"
#include "Graphics\Model.h"
#include <string>

class RenderComponent : public Component
{
public:
    RenderComponent(std::string modelFileName, GameObject *owner);
    RenderComponent(std::string modelFileName, Material *mat, GameObject *owner, CBY::Vector4 colorAlpha = CBY::Vector4());

    RenderComponent(Model *Model, GameObject* owner);
    ~RenderComponent();

    //Whether or not to render the component
    bool renderComponent = true;
    bool transparent = false;
    Model *pModel;
};
