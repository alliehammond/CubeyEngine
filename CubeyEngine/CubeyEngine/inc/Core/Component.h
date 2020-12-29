#pragma once

#include <string>

class GameObject;

class Component
{
public:
    Component(GameObject *Owner) : owner(Owner) {}
    Component(GameObject *Owner, std::string compName) : owner(Owner), name(compName) {}
    virtual ~Component() {}
    virtual void Update() {}
    //Called after all other components have called update()
    virtual void PostUpdate() {}
    void DeleteComponent() { deleteFlag = true; }
    std::string name = "Component";
    GameObject *owner;

private:
    //If delete flag is true, component will be deleted at start of gameobject's update()
    bool deleteFlag = false;
};
