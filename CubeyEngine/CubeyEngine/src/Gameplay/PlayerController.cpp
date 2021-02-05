#include "EnginePCH.h"
#include "Gameplay\PlayerController.h"

PlayerController::PlayerController(GameObject* owner) : Component(owner, "PlayerController")
{
    pTrans = owner->GetComponent<Transform>();
}

PlayerController::~PlayerController() { }

void PlayerController::Update(float dt)
{
    //Update player position

    //Player rotation
    if(InputSystem::GetKeyDown('A'))
        pTrans->rot.y += turnSpeed * dt;
    if(InputSystem::GetKeyDown('D'))
        pTrans->rot.y -= turnSpeed * dt;
    if(InputSystem::GetKeyDown('Q'))
        pTrans->rot.x += turnSpeed * dt;
    if(InputSystem::GetKeyDown('E'))
        pTrans->rot.x -= turnSpeed * dt;
    //Limit pitch to a 180 degree range
    if(pTrans->rot.x > PI / 2.0f)pTrans->rot.x = PI / 2.0f;
    if(pTrans->rot.x < -PI / 2.0f)pTrans->rot.x = -PI / 2.0f;

    //Player movement
    CBY::Vector forwardVec(cos(pTrans->rot.y), sin(pTrans->rot.x), sin(pTrans->rot.y));
    if(InputSystem::GetKeyDown('W'))
    {
        pTrans->pos += forwardVec * moveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('S'))
    {
        pTrans->pos -= forwardVec * moveSpeed * dt;
    }

    GraphicsSystem::SetCameraTrans(pTrans);
}
