#pragma once

class CubeySystem
{
public:
    CubeySystem() { }
    virtual ~CubeySystem() { }
    virtual void Update() = 0;
};

enum CubeySystems
{
    LOGGINGSYSTEM,
    SYSTEMCOUNT
};
