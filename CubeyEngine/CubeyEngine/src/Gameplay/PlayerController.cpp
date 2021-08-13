#include "EnginePCH.h"
#include "Gameplay\PlayerController.h"

Transform *PlayerController::pTrans = 0;

PlayerController::PlayerController(GameObject* owner) : Component(owner, "PlayerController")
{
    pTrans = owner->GetComponent<Transform>();
}

PlayerController::~PlayerController()
{ }

void PlayerController::Update(float dt)
{
    //Update player position
    float activeMoveSpeed = moveSpeed;
    if(InputSystem::GetKeyDown(KeyCode::CTRL))
        activeMoveSpeed = slowMoveSpeed;
    if(InputSystem::GetKeyDown(KeyCode::SHIFT))
        activeMoveSpeed = fastMoveSpeed;

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
        pTrans->pos += forwardVec * activeMoveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('S'))
    {
        pTrans->pos -= forwardVec * activeMoveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('D'))
    {
        pTrans->pos += rightVec * activeMoveSpeed * dt;
    }
    if(InputSystem::GetKeyDown('A'))
    {
        pTrans->pos -= rightVec * activeMoveSpeed * dt;
    }
    //LOGDEBUG("X: " + std::to_string(pTrans->pos.x) + ", Y: " + std::to_string(pTrans->pos.y) + ", Z: " + std::to_string(pTrans->pos.z));

    GraphicsSystem::SetCameraTrans(pTrans);
    
    //Block placement
    if(InputSystem::GetKeyPressed('0'))
    {
        PlaceBlock(BlockType::Air);
    }
    if(InputSystem::GetKeyPressed('1'))
    {
        PlaceBlock(BlockType::Dirt);
    }
    if(InputSystem::GetKeyPressed('2'))
    {
        PlaceBlock(BlockType::Grass);
    }
    if(InputSystem::GetKeyPressed('3'))
    {
        PlaceBlock(BlockType::Stone);
    }
    if(InputSystem::GetKeyPressed('4'))
    {
        PlaceBlock(BlockType::Log);
    }
    if(InputSystem::GetKeyPressed('5'))
    {
        PlaceBlock(BlockType::Leaf);
    }
    if(InputSystem::GetKeyPressed('6'))
    {
        PlaceBlock(BlockType::Plank);
    }
}

Transform* PlayerController::GetPlayerTrans()
{
    return pTrans;
}

std::tuple<int, int, int> PlayerController::GetBlockPlacementCoord(bool deleteBlockCoord)
{
    CBY::Vector forwardVec(cos(pTrans->rot.y) * cos(pTrans->rot.x), sin(pTrans->rot.x), sin(pTrans->rot.y) * cos(pTrans->rot.x));
    
    CBY::Vector camPos = pTrans->pos;
    float interpolateAmount = 0.2f;
    //Interpolate to find ideal block placement
    for(float i = interpolateAmount; i <= CBYDefines::MaxBlockPlaceDist; i += interpolateAmount)
    {
        if(TerrainManagerSystem::GetBlockInLoadedChunk(int(std::floor(camPos.x + forwardVec.x * i)), int(std::floor(camPos.y + forwardVec.y * i)), int(std::floor(camPos.z + forwardVec.z * i))) 
            != BlockType::Air)
        {
            if(!deleteBlockCoord)i -= interpolateAmount;
            //Return previous block since it is (in most circumstances) the correct air block if not deleting the block
            return std::tuple<int, int, int>(int(std::floor(camPos.x + forwardVec.x * i)), int(std::floor(camPos.y + forwardVec.y * i)), int(std::floor(camPos.z + forwardVec.z * i)));
        }
    }

    
    //If no valid block placement found, default to placing block at 5 distance away
    forwardVec *= 5;
    int curBlockX = int(std::floor(pTrans->pos.x));
    int curBlockY = int(std::floor(pTrans->pos.y));
    int curBlockZ = int(std::floor(pTrans->pos.z));

    return std::tuple<int, int, int>(int(curBlockX + std::round(forwardVec.x)), int(curBlockY + std::round(forwardVec.y)), int(curBlockZ + std::round(forwardVec.z)));
}

void PlayerController::PlaceBlock(BlockType type)
{
    std::tuple<int, int, int> pos;
    if(type == BlockType::Air)
        pos = GetBlockPlacementCoord(true);
    else
        pos = GetBlockPlacementCoord(false);
    if(std::get<1>(pos) < 256 && std::get<1>(pos) >= 0)
    {
        TerrainManagerSystem::SetBlock(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos), type);
    }
}
