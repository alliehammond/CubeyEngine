#include "EnginePCH.h"
#include "Graphics\Transform.h"

Transform::Transform(GameObject* owner) : Component(owner), scale(1.0f, 1.0f, 1.0f)
{}

Transform::Transform(float xPos, float yPos, float zPos, GameObject* owner) : Component(owner), pos(xPos, yPos, zPos), scale(1.0f, 1.0f, 1.0f)
{}
