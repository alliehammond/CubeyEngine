#include "EnginePCH.h"
#include "Core\ObjectManagerSystem.h"

std::vector<GameObject *> ObjectManagerSystem::gameObjects;

ObjectManagerSystem::ObjectManagerSystem()
{
    LOGDEBUG("Object manager system initialized!");
}

ObjectManagerSystem::~ObjectManagerSystem()
{
    //Destroy all remaining game objects
    for(auto &it : gameObjects)
    {
        delete it;
    }
    gameObjects.clear();
}

void ObjectManagerSystem::Update(float dt)
{
    //Delete game objects with deletion flag set
    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](GameObject *it)
    { 
        //Delete game object and return true to delete pointer from vector
        if(it->GetDeletionFlag())
        {
            delete it;
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

GameObject *ObjectManagerSystem::CreateObject(GameObject* obj)
{
    gameObjects.push_back(obj);
    return obj;
}
