#pragma once

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
    SYSTEMCOUNT
};
