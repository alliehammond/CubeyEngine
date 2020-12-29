#include "EnginePCH.h"
#include "Graphics\Transform.h"

Transform::Transform(GameObject* owner) : Component(owner), scale(1.0f, 1.0f, 1.0f)
{}
