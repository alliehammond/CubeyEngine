#pragma once
#include "Core\CubeySystem.h"
#include "Core\GameObject.h"

#include <vector>

//Manages creation and deletion of objects
class ObjectManagerSystem : public CubeySystem
{
public:
    ObjectManagerSystem();
    ~ObjectManagerSystem();
    void Update(float dt);
    static GameObject *CreateObject(GameObject *obj);

private:
    static std::vector<GameObject *> gameObjects;
    //Game objects created current frame, to be copied to gameObjects after all updates called
    static std::vector<GameObject *> newGameObjects;
    friend class GraphicsSystem;
};
