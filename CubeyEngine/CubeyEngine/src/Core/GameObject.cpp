#include "EnginePCH.h"
#include "Core\GameObject.h"
#include "Graphics\RenderComponent.h"

GameObject::GameObject()
{
    AddComponent(std::make_unique<Transform>(this));
}

GameObject::GameObject(std::string objName) : name(objName)
{
    AddComponent(std::make_unique<Transform>(this));
}

GameObject::~GameObject()
{ }

//Each game object calls update() and postupdate() of each of their components
void GameObject::Update(float dt)
{
    for(auto it = components.begin(); it != components.end(); ++it)
    {
        if(it->second->GetDeleteFlag())components.erase(it--);
    }
    for(auto& it : components)
    {
        it.second->Update(dt);
    }
    for(auto& it : components)
    {
        it.second->PostUpdate(dt);
    }
}

void GameObject::Delete()
{
    deletionFlag = true;
}

