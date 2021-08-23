#pragma once
#include "Graphics\Material.h"

enum class BlockType : unsigned char
{
    Air = 0,
    Dirt,
    Grass,
    Stone,
    Log,
    Leaf,
    Plank,
    Glass,
    FlowerLeaf,
    BLOCKCOUNT
};

namespace CBY
{
    //Pointer that is returned must be manually deleted
    Material *GetBlockMaterial(BlockType bType);
};
