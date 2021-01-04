#include "EnginePCH.h"
#include "Core\GameObject.h"
#include "Graphics\RenderComponent.h"

GameObject::GameObject()
{
    AddComponent(new Transform(this));
}

GameObject::GameObject(std::string objName) : name(objName)
{
    AddComponent(new Transform(this));
}

GameObject::~GameObject()
{
    //Delete all the components
    for(auto &it : components)
    {
        delete it.second;
    }
}

//Each game object calls update() and postupdate() of each of their components
void GameObject::Update()
{
    for(auto& it : components)
    {
        if(!it.second)continue;
        it.second->Update();
    }

    for(auto& it : components)
    {
        if(!it.second)continue;
        it.second->PostUpdate();
    }
}

void GameObject::Delete()
{
    deletionFlag = true;
}

