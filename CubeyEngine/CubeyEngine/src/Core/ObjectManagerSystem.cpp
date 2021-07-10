#include "EnginePCH.h"
#include "Core\ObjectManagerSystem.h"

std::vector<std::unique_ptr<GameObject>> ObjectManagerSystem::gameObjects;

ObjectManagerSystem::ObjectManagerSystem()
{
    LOGDEBUG("Object manager system initialized!");
}

ObjectManagerSystem::~ObjectManagerSystem()
{
    gameObjects.clear();
}

void ObjectManagerSystem::Update(float dt)
{
    //Delete game objects with deletion flag set
    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](std::unique_ptr<GameObject> &it)
    { 
        if(it->GetDeletionFlag())
        {
            return true;
        }
        return false;
    }), gameObjects.end());
    //Call game object updates
    for(auto &it : gameObjects)
    {
        it->Update(dt);
    }
}

GameObject *ObjectManagerSystem::CreateObject(std::unique_ptr<GameObject> obj)
{
    GameObject *ret = obj.get();
    gameObjects.push_back(std::move(obj));
    return ret;
}
