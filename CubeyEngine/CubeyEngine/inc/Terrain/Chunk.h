#pragma once

#include "Graphics\Model.h"
#include "Core\GameObject.h"
#include "Terrain\BlockType.h"
#include <vector>

class Chunk
{
public:
    Chunk(int xLoc, int yLoc, int zLoc);
    ~Chunk();
    //Chunk location (chunks next to each other increment by 1 not by absolute location)
    int x, y, z;
    BlockType GetBlockChunkRelative(int x, int y, int z);
    
private:
    //Used for generating mesh
    int numBlocks = 0;

    void LoadChunk();
    void CreateChunkMesh();
    void SetBlockChunkRelative(int x, int y, int z, BlockType type);
    std::vector<BlockType> blocks;
    GameObject *blockTerrain;
};
