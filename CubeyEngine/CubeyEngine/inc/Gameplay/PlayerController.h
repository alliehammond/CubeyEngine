#pragma once
#include "Core\Component.h"

class PlayerController : public Component
{
public:
    PlayerController(GameObject* owner);
    ~PlayerController();
    void Update(float dt);
    static Transform *GetPlayerTrans();

private:
    //Pointer to transform comp on player object
    static Transform *pTrans;
    float slowMoveSpeed = 15.0f;
    float moveSpeed = 30.0f;
    float fastMoveSpeed = 60.0f;
    float turnSpeed = 0.0006f;
};

