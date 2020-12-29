#pragma once

// **Note: All game systems are designed to be singletons

class CubeySystem
{
public:
    CubeySystem() { }
    virtual ~CubeySystem() { }
    virtual void Update(float dt) = 0;
};

enum CubeySystems
{
    LOGGINGSYSTEM,
    OBJECTMANAGERSYSTEM,
    GRAPHICSSYSTEM,
    SYSTEMCOUNT
};
