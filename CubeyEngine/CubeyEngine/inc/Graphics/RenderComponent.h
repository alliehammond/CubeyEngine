#pragma once

#include "Core\Component.h"
#include "Graphics\Model.h"
#include <string>

class RenderComponent : public Component
{
public:
    RenderComponent(std::string modelFileName, GameObject *owner);
    ~RenderComponent();

    Model *pModel;
};
