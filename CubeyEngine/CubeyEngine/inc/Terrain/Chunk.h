#pragma once

#include "Graphics\Model.h"
#include "Core\GameObject.h"
#include "Terrain\BlockType.h"
#include <vector>

class Chunk
{
public:
    Chunk(short xLoc, short yLoc, short zLoc);
    ~Chunk();
    //Chunk location (chunks next to each other increment by 1 not by absolute location)
    short x, y, z;
    BlockType GetBlockChunkRelative(short x, short y, short z);
    
private:
    //Used for generating mesh
    int numBlocks = 0;

    void LoadChunk();
    void CreateChunkMesh();
    void SetBlockChunkRelative(short x, short y, short z, BlockType type);
    std::vector<BlockType> blocks;
    GameObject *blockTerrain;

    friend class TerrainManagerSystem;
};
