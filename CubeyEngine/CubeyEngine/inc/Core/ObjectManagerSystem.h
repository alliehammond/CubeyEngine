#pragma once
#include "Core\CubeySystem.h"
#include "Core\GameObject.h"

#include <vector>
#include <memory>

#define CreateGameObject(a) ObjectManagerSystem::CreateObject(std::make_unique<GameObject>(a))

//Manages creation and deletion of objects
class ObjectManagerSystem : public CubeySystem
{
public:
    ObjectManagerSystem();
    ~ObjectManagerSystem();
    void Update(float dt);
    static GameObject *CreateObject(std::unique_ptr<GameObject> obj);

private:
    static std::vector<std::unique_ptr<GameObject>> gameObjects;
    friend class GraphicsSystem;
};
