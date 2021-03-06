#pragma once

#include "Core\Component.h"
#include "Graphics\Transform.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>

class ObjectManagerSystem;

class GameObject
{
public:
    GameObject();
    GameObject(std::string objName);
    ~GameObject();

    //Each game object calls update() and postupdate() of each of their components
    void Update(float dt);
    //Deletes game object at the start of next frame
    void Delete();

    bool GetDeletionFlag() { return deletionFlag; }

    std::string name = "GameObject";

private:
    //While iterating over components some can be nullptrs
    std::unordered_map<std::type_index, Component *> components;
    //Game object will be destroyed at end of frame if true
    bool deletionFlag = false;

public:
    template <typename T>
    T *GetComponent()
    {
        return (T*)components[std::type_index(typeid(T))];
    }
    template <typename T>
    bool HasComponent()
    {
        if(GetComponent<T>() != 0)
            return true;
        return false;
    }
    template <typename T>
    T *AddComponent(T *comp)
    {
        components[std::type_index(typeid(T))] = comp;
        return comp;
    }
    template <typename T>
    void DeleteComponent()
    {
        components[std::type_index(typeid(T))]->DeleteComponent();
        components.erase(std::type_index(typeid(T)));
    }
};
