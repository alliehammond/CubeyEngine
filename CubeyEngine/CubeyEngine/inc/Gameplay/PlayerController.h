#pragma once
#include "Core\Component.h"
#include <tuple>

class PlayerController : public Component
{
public:
    PlayerController(GameObject* owner);
    ~PlayerController();
    void Update(float dt);
    static Transform *GetPlayerTrans();

private:
    //Returns coordinate of block placement target (block coordinates)
    std::tuple<int, int, int> GetBlockPlacementCoord();
    void PlaceBlock(BlockType type);

    //Pointer to transform comp on player object
    static Transform *pTrans;
    float slowMoveSpeed = 5.0f;
    float moveSpeed = 15.0f;
    float fastMoveSpeed = 45.0f;
    float turnSpeed = 0.0006f;

    GameObject *pBlockPlacementOutline;
    bool showBlockPlaceOutline;
};

