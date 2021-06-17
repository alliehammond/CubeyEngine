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
    //Sets a block in the chunk, if regenMesh is true regenerates the mesh after
    void SetBlock(short x, short y, short z, BlockType type, bool regenMesh = true);
    bool IsAirChunk() const { return empty || numBlocks == 0; }

private:
    //Number of non-air blocks
    int numBlocks = 0;
    bool empty;

    void LoadChunk();
    //Creates the chunk mesh once blocks vector has been filled and numBlocks has been set to number of non-air blocks
    void CreateChunkMesh();
    //Don't use this function to set blocks in empty chunk
    void SetBlockChunkRelative(short x, short y, short z, BlockType type);
    std::vector<BlockType> blocks;
    GameObject *blockTerrain;

    friend class TerrainManagerSystem;
};
