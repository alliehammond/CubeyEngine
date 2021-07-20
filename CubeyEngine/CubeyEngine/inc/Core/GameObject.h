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
        auto it = components.find(std::type_index(typeid(T)));
        if(it == components.end())return 0;
        return (T*)it->second;
    }
    template <typename T>
    bool HasComponent()
    {
        auto it = components.find(std::type_index(typeid(T)));
        if(it == components.end())return false;
        return true;
    }
    //If component already exists, return that component
    template <typename T>
    T *AddComponent(T *comp)
    {
        auto it = components.find(std::type_index(typeid(T)));
        if(it != components.end())
        {
            LOGWARNING("Adding already existing component, possible memory leak!");
            return (T *)it->second;
        }

        components[std::type_index(typeid(T))] = comp;
        return comp;
    }
    template <typename T>
    void DeleteComponent()
    {
        if(!HasComponent<T>())return;
        components[std::type_index(typeid(T))]->DeleteComponent();
        components.erase(std::type_index(typeid(T)));
    }
};
