#include "EnginePCH.h"
#include "Terrain\TerrainManagerSystem.h"
#include "Gameplay\PlayerController.h"
#include "Terrain\Chunk.h"
#include <fstream>

std::unordered_map<unsigned __int64, Chunk*> TerrainManagerSystem::loadedChunks;

const std::string terrainDataPath = "../resources/terrainData/";

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
    //Save loaded chunks
    if(InputSystem::GetKeyPressed('M'))
    {
        SaveLoadedChunks();
    }

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
    
    static float timer = 1.0f;
    if(timer <= 0.0f)
    {
        auto it = loadedChunks.begin();
        //Delete chunks that are out of render delete range
        while(it != loadedChunks.end())
        {
            Chunk *pChunk = it->second;
            if(pChunk->x < pcx - CBYDefines::RenderDeleteDistance || pChunk->x > pcx + CBYDefines::RenderDeleteDistance ||
                pChunk->z < pcz - CBYDefines::RenderDeleteDistance || pChunk->z > pcz + CBYDefines::RenderDeleteDistance)
            {
                delete pChunk;
                it = loadedChunks.erase(it);
            }
            else
            {
                it++;
            }
        }
        timer = 1.0f;
    }
    else
        timer -= dt;
}

Chunk* TerrainManagerSystem::GetChunk(short cX, short cY, short cZ)
{
    auto it = loadedChunks.find(HashChunkCoord(cX, cY, cZ));
    if(it == loadedChunks.end())return 0;
    return it->second;
}

//Rounds a single coordinate value to the region coordinate
short GetRegionShortCoordFromShort(short n)
{
    if(n >= 0)
    {
        return (n / 4) * 4;
    }
    if(n % 4 == 0)
        return n;
    return ((n / 4) * 4) - 4;
}

//Region coordinate is defined as the upper left chunk coordinate (min x, y, z)
std::tuple<short, short, short> TerrainManagerSystem::GetRegionCoordinateFromChunk(Chunk *pChunk)
{
    std::tuple<short, short, short> ret;
    
    std::get<0>(ret) = GetRegionShortCoordFromShort(pChunk->x);
    std::get<1>(ret) = GetRegionShortCoordFromShort(pChunk->y);
    std::get<2>(ret) = GetRegionShortCoordFromShort(pChunk->z);

    return ret;
}

void TerrainManagerSystem::SaveLoadedChunks()
{
    std::vector<std::pair<unsigned __int64, std::vector<Chunk *>>> regions;
    //**Loop through loaded chunks, adding them to corresponding regions (or creating new regions if not in regions vector)
    for(auto &it : loadedChunks)
    {
        std::tuple<short, short, short> regionCoord = GetRegionCoordinateFromChunk(it.second);
        unsigned __int64 rHash = HashChunkCoord(std::get<0>(regionCoord), std::get<1>(regionCoord), std::get<2>(regionCoord));
        bool rHashFound = false;
        //Find rHash, if not found add new region
        for(auto &jt : regions)
        {
            if(jt.first == rHash)
            {
                jt.second.push_back(it.second);
                rHashFound = true;
                break;
            }
        }
        if(!rHashFound)
        {
            regions.push_back(std::pair<unsigned __int64, std::vector<Chunk *>>(rHash, std::vector<Chunk *>()));
            regions.back().second.push_back(it.second);
        }
    }


    //**Call SaveRegion() on each region
    for(auto &it : regions)
    {
        std::tuple<short, short, short> coord = GetRegionCoordinateFromChunk(it.second[0]);
        SaveRegion(std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), it.second);
    }
}

unsigned __int64 TerrainManagerSystem::HashChunkCoord(short cX, short cY, short cZ)
{
    //Create unique hash
    unsigned __int64 x = cX - SHRT_MIN;
    unsigned __int64 y = cY - SHRT_MIN;
    unsigned __int64 z = cZ - SHRT_MIN;

    return x + (y << 16) + (z << 32);
}

//Each region is a 4x4x4 group of chunks
void TerrainManagerSystem::SaveRegion(short x, short y, short z, std::vector<Chunk *> &chunks)
{
    unsigned __int64 hash = HashChunkCoord(x, y, z);
    std::string fileName = "region" + std::to_string(hash) + ".dat";
    std::string path = terrainDataPath + fileName;
    //Open region file in binary mode, deleting file if it already exists
    std::ofstream regionFile;
    regionFile.open(path, std::ios::trunc | std::ios::binary);

    //**Order the chunks into same order as chunk data for reading efficiency(see RegionFileFormat.txt)
    struct chunkLessThan
    {
        //Returns true if first object less than second in terms of intended x,y,z position order
        bool operator() (const Chunk *chunk1, const Chunk *chunk2)
        {
            if(chunk1->z < chunk2->z)
                return true;
            if(chunk2->z < chunk1->z)
                return false;
            if(chunk1->y < chunk2->y)
                return true;
            if(chunk2->y < chunk1->y)
                return false;
            if(chunk1->x < chunk2->x)
                return true;
            return false;

        };
    };
    //Sort chunk vector
    std::sort(chunks.begin(), chunks.end(), chunkLessThan());
    
    //Vector that associates each respective chunk with its corresponding boundary offset
    std::vector<std::pair<Chunk *, int>> chunksOffsets;
    //Copy data into the vector
    for(auto it : chunks)
    {
        chunksOffsets.push_back(std::pair<Chunk *, int>(it, 0));
    }

    //**Preprocess chunks to find their chunk data sizes and assign which 256 byte boundary they will start on
    std::vector<int> boundaryOffsets;
    //Keep track of expected next chunk coordinate, so nonexistant ('air') chunks can be added to the header
    int expectedX = 0 + x, expectedY = 0 + y, expectedZ = 0 + z;
    //Start offset at 1 since the header is 256 bytes (1 * 256 >= 256)
    int curOffset = 1;

    auto jt = chunksOffsets.begin();
    while(expectedZ < 4 + z)
    {
        if(jt != chunksOffsets.end() && (*jt).first->x == expectedX && (*jt).first->y == expectedY && (*jt).first->z == expectedZ)
        {
            //Chunk found in loaded chunks, calculate its size
            //Note this will become a more complex calculation if compression like RLE is used
            int chunkSize = 64 * 64 * 64;
            (*jt).second = curOffset;
            boundaryOffsets.push_back(curOffset);
            curOffset += chunkSize / 256;
            if(chunkSize % 256 != 0)++curOffset;
            jt++;
        }
        else
        {
            //Chunk not found, assume it's an air chunk
            boundaryOffsets.push_back(0);
        }

        //Increment expected coordinates
        ++expectedX;
        if(expectedX > 3 + x)
        {
            ++expectedY;
            expectedX = 0 + x;
        }
        if(expectedY > 3 + y)
        {
            ++expectedZ;
            expectedY = 0 + y;
        }
    }


    //**Write header data to file
    for(auto &it : boundaryOffsets)
    {
        //Write 4 byte offsets for each chunk offset
        regionFile.write((char *)&it, 4);
    }
    
    //**Write chunk data in correct order to file, ensuring each chunk starts it's corresponding 256 byte boundary
    for(auto &it : chunksOffsets)
    {
        //Seek to corresponding 256 byte boundary
        regionFile.seekp(it.second * 256);

        //Write chunk data
        std::vector<unsigned char> buffer;
        for(auto &kt : it.first->blocks)
        {
            buffer.push_back(unsigned char(kt));
        }
        regionFile.write((char *)(&(buffer[0])), buffer.size());
    }


    regionFile.close();
    LOGDEBUG("Saved region (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") " + fileName + "!");
}