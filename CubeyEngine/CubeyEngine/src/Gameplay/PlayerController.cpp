#include "EnginePCH.h"
#include "Gameplay\PlayerController.h"

Transform *PlayerController::pTrans;

PlayerController::PlayerController(GameObject* owner) : Component(owner, "PlayerController")
{
    pTrans = owner->GetComponent<Transform>();
}

PlayerController::~PlayerController() { }

void PlayerController::Update(float dt)
{
    //Update player position

    //Player rotation
    pTrans->rot.y -= InputSystem::GetMouseDeltaX() * turnSpeed;
    pTrans->rot.x += InputSystem::GetMouseDeltaY() * turnSpeed;

    //Limit pitch to a 180(ish) degree range
    if(pTrans->rot.x > PI / 2.1f)pTrans->rot.x = PI / 2.1f;
    if(pTrans->rot.x < -PI / 2.1f)pTrans->rot.x = -PI / 2.1f;

    //Player movement
    CBY::Vector forwardVec(cos(pTrans->rot.y) * cos(pTrans->rot.x), sin(pTrans->rot.x), sin(pTrans->rot.y) * cos(pTrans->rot.x));
    CBY::Vector rightVec(cos(pTrans->rot.y - PI / 2.0f), 0.0f, sin(pTrans->rot.y - PI / 2.0f));
    if(InputSystem::GetKeyDown('W'))
    {
        pTrans->pos += forwardVec * moveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('S'))
    {
        pTrans->pos -= forwardVec * moveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('D'))
    {
        pTrans->pos += rightVec * moveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('A'))
    {
        pTrans->pos -= rightVec * moveSpeed * dt;
    }

    GraphicsSystem::SetCameraTrans(pTrans);
}

Transform* PlayerController::GetPlayerTrans()
{
    return pTrans;
}
