#include "EnginePCH.h"
#include "Terrain\TerrainManagerSystem.h"
#include "Gameplay\PlayerController.h"
#include "Terrain\Chunk.h"

std::unordered_map<unsigned __int64, Chunk*> TerrainManagerSystem::loadedChunks;

TerrainManagerSystem::TerrainManagerSystem()
{

}

TerrainManagerSystem::~TerrainManagerSystem()
{
    //Destroy chunks
    for(auto &it : loadedChunks)
    {
        delete it.second;
    }
}

void TerrainManagerSystem::Update(float dt)
{
    CBY::Vector ptrans = PlayerController::GetPlayerTrans()->pos;
    //Player chunk coords
    short pcx = short(std::round(PlayerController::GetPlayerTrans()->pos.x / (CBYDefines::ChunkSize * CBYDefines::BlockSize)));
    short pcy = short(std::round(PlayerController::GetPlayerTrans()->pos.y / (CBYDefines::ChunkSize * CBYDefines::BlockSize)));
    short pcz = short(std::round(PlayerController::GetPlayerTrans()->pos.z / (CBYDefines::ChunkSize * CBYDefines::BlockSize)));

    //Update chunk load list based on player position
    //Temp - don't iterate on y axis
    for(short i = pcx - CBYDefines::RenderDistance; i <= pcx + CBYDefines::RenderDistance; ++i)
    {
        for(short j = 0; j <= 0; ++j)
        {
            for(short k = pcz - CBYDefines::RenderDistance; k <= pcz + CBYDefines::RenderDistance; ++k)
            {
                //Check if chunk is either loaded or in load queue
                if(GetChunk(i, j, k) != 0)continue;
                auto tuple = std::make_tuple(i, j, k);
                auto it = std::find(chunkLoadList.begin(), chunkLoadList.end(), tuple);
                if(it != chunkLoadList.end())continue;
                //Chunk not found, add to load list
                chunkLoadList.push_back(tuple);
            }
        }
    }

    //Pop tuples from load list and create chunks out of them
    for(int i = 0; i < CBYDefines::ChunkLoadsPerFrame && !chunkLoadList.empty(); ++i)
    {
        std::tuple<short, short, short> tuple = chunkLoadList.back();
        chunkLoadList.pop_back();
        Chunk *newChunk = new Chunk(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        loadedChunks[HashChunkCoord(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple))] = newChunk;
    }
    
}

Chunk* TerrainManagerSystem::GetChunk(short cX, short cY, short cZ)
{
    auto it = loadedChunks.find(HashChunkCoord(cX, cY, cZ));
    if(it == loadedChunks.end())return 0;
    return it->second;
}

unsigned __int64 TerrainManagerSystem::HashChunkCoord(short cX, short cY, short cZ)
{
    //Create unique hash
    unsigned __int64 x = cX - SHRT_MIN;
    unsigned __int64 y = cY - SHRT_MIN;
    unsigned __int64 z = cZ - SHRT_MIN;

    return x + (y << 16) + (z << 32);
}
