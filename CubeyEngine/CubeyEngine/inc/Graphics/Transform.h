#pragma once
#include "Core\Component.h"
#include "CYMath\Vector.h"

class GameObject;

class Transform : public Component
{
public:
    Transform(GameObject *owner);
    Transform(float xPos, float yPos, float zPos, GameObject *owner);
    //Rotation uses radians
    //rot.x,y,z -> rot.pitch,yaw,roll
    CBY::Vector pos, scale, rot;
};
