#pragma once
#include "Core\Component.h"
#include "CYMath\Vector.h"

class GameObject;

class Transform : public Component
{
public:
    Transform(GameObject *owner);
    CBY::Vector pos, scale, rot;
};
