#include "EnginePCH.h"
#include "Core\GameObject.h"

GameObject::GameObject()
{ }

GameObject::GameObject(std::string objName) : name(objName)
{
    std::string i = "Creating object " + name;
    LOGDEBUG(i);
}

GameObject::~GameObject()
{
    std::string i = "Deleting object " + name;
    LOGDEBUG(i);
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
        it.second->Update();
    }

    for(auto& it : components)
    {
        it.second->PostUpdate();
    }
}

void GameObject::Delete()
{
    deletionFlag = true;
}

