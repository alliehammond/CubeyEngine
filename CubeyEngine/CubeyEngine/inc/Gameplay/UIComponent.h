#pragma once

#include "Core\Component.h"

class UIComponent : public Component
{
public:
    UIComponent(GameObject *owner);
    ~UIComponent();
    void PostUpdate(float dt);
};
