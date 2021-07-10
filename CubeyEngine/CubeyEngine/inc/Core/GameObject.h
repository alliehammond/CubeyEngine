#pragma once

#include "Core\Component.h"
#include "Graphics\Transform.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>

class ObjectManagerSystem;

//All GameObjects start with a transform component
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
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    //Game object will be destroyed at end of frame if true
    bool deletionFlag = false;

public:
    //Returns null if component doesn't exist
    template <typename T>
    T *GetComponent()
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it == components.end())
            return 0;
        return (T*)((*it).second.get());
    }
    template <typename T>
    bool HasComponent()
    {
        if(components.find(std::type_index(typeid(T))) == components.end())
            return false;
        return true;
    }
    template <typename T>
    T *AddComponent(std::unique_ptr<T> comp)
    {
        T *ret = comp.get();
        if(ret == 0)return 0;
        if(HasComponent<T>())
        {
            LOGERROR("Double creation of component!");
            return 0;
        }

        components[std::type_index(typeid(T))] = std::move(comp);
        return ret;
    }
    template <typename T>
    void DeleteComponent()
    {
        auto &it = components.find(std::type_index(typeid(T)));
        if(it != components.end())it->second->DeleteComponent();
    }
};
