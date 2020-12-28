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
};
