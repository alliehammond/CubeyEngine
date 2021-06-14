#pragma once

#include "Graphics\Model.h"
#include "Core\GameObject.h"
#include "Terrain\BlockType.h"
#include <vector>

class Chunk
{
public:
    Chunk(short xLoc, short yLoc, short zLoc, bool empty_);
    ~Chunk();
    //Chunk location (chunks next to each other increment by 1 not by absolute location)
    short x, y, z;
    BlockType GetBlockChunkRelative(short x, short y, short z);
    //Create public function for setting blocks: check if chunk is empty and if so resize blocks vector/set empty to false, also check if adding a non air block and inc/dec numBlocks
    
private:
    //Number of non-air blocks
    int numBlocks = 0;
    bool empty;

    void LoadChunk();
    void CreateChunkMesh();
    //Don't use this function to set blocks in empty chunk
    void SetBlockChunkRelative(short x, short y, short z, BlockType type);
    std::vector<BlockType> blocks;
    GameObject *blockTerrain;

    friend class TerrainManagerSystem;
};
