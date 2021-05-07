#pragma once
#include "Core\Component.h"

class PlayerController : public Component
{
public:
    PlayerController(GameObject* owner);
    ~PlayerController();
    void Update(float dt);

private:
    //Pointer to transform comp on player object
    Transform *pTrans;
    float moveSpeed = 5.0f;
    float turnSpeed = 0.0006f;
};

