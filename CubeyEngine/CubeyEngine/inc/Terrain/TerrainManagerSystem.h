#pragma once
#include "Core\CubeySystem.h"
#include <unordered_map>

class Chunk;

class TerrainManagerSystem : public CubeySystem
{
public:
    TerrainManagerSystem();
    ~TerrainManagerSystem();
    void Update(float dt);
    //Use chunk coordinates not world coords; returns null if chunk not loaded/doesn't exist
    static Chunk* GetChunk(short cX, short cY, short cZ);
    void SaveLoadedChunks();

private:
    static unsigned __int64 HashChunkCoord(short cX, short cY, short cZ);
    std::tuple<short, short, short> GetRegionCoordinateFromChunk(Chunk* pChunk);
    //Each region is a 4x4x4 group of chunks
    void SaveRegion(short x, short y, short z, std::vector<Chunk *> &chunks);

    static std::unordered_map<unsigned __int64, Chunk *> loadedChunks;
    //List of chunks that need to be loaded
    std::vector<std::tuple<short, short, short>> chunkLoadList;
};
