#pragma once

#include "Graphics\Model.h"
#include "Core\GameObject.h"
#include <vector>

class Chunk
{
public:
    Chunk(int xLoc, int yLoc, int zLoc);
    ~Chunk();
    //Chunk location (chunks next to each other increment by 1 not by absolute location)
    int x, y, z;
private:
    std::vector<GameObject *> blockTerrain;
};
