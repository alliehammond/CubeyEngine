#include "EnginePCH.h"
#include "Terrain\TerrainManagerSystem.h"
#include "Gameplay\PlayerController.h"
#include "Terrain\Chunk.h"
#include <fstream>

std::unordered_map<unsigned __int64, Chunk*> TerrainManagerSystem::loadedChunks;
std::vector<std::pair<unsigned __int64, std::ifstream *>> TerrainManagerSystem::openRegionFiles;

const std::string terrainDataPath = "../resources/terrainData/";

TerrainManagerSystem::TerrainManagerSystem()
{ }

TerrainManagerSystem::~TerrainManagerSystem()
{
    //Destroy chunks
    for(auto &it : loadedChunks)
    {
        delete it.second;
    }
    
    CloseOpenRegionFiles();
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
    for(short i = pcx - CBYDefines::RenderDistance; i <= pcx + CBYDefines::RenderDistance; ++i)
    {
        for(short j = 0; j < 4; ++j)
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
        LoadAndCreateChunk(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }

    //Close open region files if the chunk load list is empty to free up file handles
    if(chunkLoadList.empty())
        CloseOpenRegionFiles();
    
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

//Gets the corresponding chunk coordinate to inputted block coordinate  
short TerrainManagerSystem::GetChunkCoordinate(int n)
{
    if(n >= 0)
    {
        return n / CBYDefines::ChunkSize;
    }
    if(n % CBYDefines::ChunkSize == 0)
        return n / CBYDefines::ChunkSize;
    return (n / CBYDefines::ChunkSize) - 1;
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
    return GetRegionCoordinateFromChunk(pChunk->x, pChunk->y, pChunk->z);
}

std::tuple<short, short, short> TerrainManagerSystem::GetRegionCoordinateFromChunk(short x, short y, short z)
{
    std::tuple<short, short, short> ret;

    std::get<0>(ret) = GetRegionShortCoordFromShort(x);
    std::get<1>(ret) = GetRegionShortCoordFromShort(y);
    std::get<2>(ret) = GetRegionShortCoordFromShort(z);

    return ret;
}

void TerrainManagerSystem::SaveLoadedChunks()
{
    std::vector<std::pair<unsigned __int64, std::vector<Chunk *>>> regions;
    //**Loop through loaded chunks, adding them to corresponding regions (or creating new regions if not in regions vector)
    for(auto &it : loadedChunks)
    {
        //Don't save the chunk if it's empty
        if(it.second->numBlocks == 0)continue;

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

    //Load any chunks that aren't currently loaded on each region so they get saved
    for(auto &it :regions)
    {
        std::tuple<short, short, short> regionCoord = GetRegionCoordinateFromChunk(it.second[0]);
        //Loop through each chunk that should exist in a region
        for(short i = std::get<0>(regionCoord); i <= std::get<0>(regionCoord) + 3; ++i)
        {
            for(short j = std::get<1>(regionCoord); j <= std::get<1>(regionCoord) + 3; ++j)
            {
                for(short k = std::get<2>(regionCoord); k <= std::get<2>(regionCoord) + 3; ++k)
                {
                    bool chunkFound = false;
                    //Loop through chunk vector to see if it is loaded
                    for(auto &jt : it.second)
                    {
                        if(jt->x == i && jt->y == j && jt->z == k)
                        {
                            chunkFound = true;
                            break;
                        }
                    }

                    if(!chunkFound)
                    {
                        //Chunk not loaded - load it
                        Chunk* loadedChunk = LoadAndCreateChunk(i, j, k);
                        it.second.push_back(loadedChunk);
                    }
                }
            }
        }
    }

    CloseOpenRegionFiles();
    //**Call SaveRegion() on each region
    for(auto &it : regions)
    {
        std::tuple<short, short, short> coord = GetRegionCoordinateFromChunk(it.second[0]);
        SaveRegion(std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), it.second);
    }
}

//Uses block coordinates
void TerrainManagerSystem::SetBlock(int x, int y, int z, BlockType type, bool regenMesh)
{
    int cx = GetChunkCoordinate(x);
    int cy = GetChunkCoordinate(y);
    int cz = GetChunkCoordinate(z);

    Chunk* pChunk = GetChunk(cx, cy, cz);
    if(!pChunk)
    {
        //Chunk not loaded - load it
        pChunk = LoadAndCreateChunk(cx, cy, cz);
    }
    pChunk->SetBlock(x - cx * CBYDefines::ChunkSize, y - cy * CBYDefines::ChunkSize, z - cz * CBYDefines::ChunkSize, type, regenMesh);
}

void TerrainManagerSystem::RegenAllChunkMeshes()
{
    for(auto &it : loadedChunks)
    {
        it.second->CreateChunkMesh();
    }
}

//Uses block coordinates, returns air if chunk is not loaded
BlockType TerrainManagerSystem::GetBlockInLoadedChunk(int x, int y, int z)
{
    int cx = GetChunkCoordinate(x);
    int cy = GetChunkCoordinate(y);
    int cz = GetChunkCoordinate(z);

    Chunk *pChunk = GetChunk(cx, cy, cz);
    if(!pChunk)return BlockType::Air;
    return pChunk->GetBlockChunkRelative(x - cx * CBYDefines::ChunkSize, y - cy * CBYDefines::ChunkSize, z - cz * CBYDefines::ChunkSize);
}

//Don't modify this function - will invalidate all region files
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
    std::string fileName = GetRegionFileName(hash);
    std::string path = GetRegionFileNameAndPath(hash);
    //Open region file in binary mode, deleting file if it already exists
    std::ofstream regionFile;
    regionFile.open(path, std::ios::trunc | std::ios::binary);

    //Remove empty chunks
    auto new_end = std::remove_if(chunks.begin(), chunks.end(), [](Chunk *chunk)
    {
        if(chunk->IsAirChunk())return true;
        return false;
    });
    chunks.erase(new_end, chunks.end());

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
    //Start offset at 1 since the header is 256 bytes
    int curOffset = CBYDefines::HeaderBoundaryOffset;

    auto jt = chunksOffsets.begin();
    while(expectedZ < 4 + z)
    {
        if(jt != chunksOffsets.end() && (*jt).first->x == expectedX && (*jt).first->y == expectedY && (*jt).first->z == expectedZ)
        {
            //Chunk found in loaded chunks, calculate its size
            //Note this will become a more complex calculation if compression like RLE is used
            int chunkSize = CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize;
            (*jt).second = curOffset;
            boundaryOffsets.push_back(curOffset);
            curOffset += chunkSize / CBYDefines::ChunkDataByteBoundarys;
            if(chunkSize % CBYDefines::ChunkDataByteBoundarys != 0)++curOffset;
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
        regionFile.seekp(it.second * CBYDefines::ChunkDataByteBoundarys);

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

//Loads chunk data from region file and creates chunk object from it
Chunk *TerrainManagerSystem::LoadAndCreateChunk(short x, short y, short z)
{
    //Check if chunk is inside openRegionFiles
    std::tuple<short, short, short> rCoord = GetRegionCoordinateFromChunk(x, y, z);
    unsigned __int64 rHash = HashChunkCoord(std::get<0>(rCoord), std::get<1>(rCoord), std::get<2>(rCoord));
    std::ifstream *pRFile = 0;

    for(auto &it : openRegionFiles)
    {
        if(it.first == rHash)
        {
            pRFile = it.second;
            break;
        }
    }
    if(!pRFile)
    {
        //Region file not loaded - load it (if it exists)
        pRFile = new std::ifstream(GetRegionFileNameAndPath(rHash), std::ifstream::binary);
        if(!(*pRFile))
        {
            //Region doesn't exist, load empty chunk
            delete pRFile;
            Chunk* newChunk = new Chunk(x, y, z, true);
            loadedChunks[HashChunkCoord(x, y, z)] = newChunk;
            return newChunk;
        }
        else
        {
            LOGDEBUG("Opened region file " + std::to_string(rHash));
            openRegionFiles.push_back(std::pair<unsigned __int64, std::ifstream *>(rHash, pRFile));
        }
    }

    //Get chunk data offset from file header (see RegionFileFormat.txt)
    unsigned int chunkOffset = 0;
    int cOffsetX = x - std::get<0>(rCoord);
    int cOffsetY = y - std::get<1>(rCoord);
    int cOffsetZ = z - std::get<2>(rCoord);
    pRFile->seekg(4 * (cOffsetX + cOffsetY * 4 + cOffsetZ * 16));
    unsigned char buf[4];
    pRFile->read((char *)buf, 4);
    chunkOffset = *((unsigned int*)(buf));


    Chunk *newChunk = new Chunk(x, y, z, !chunkOffset);
    //If chunkOffset is 0 it indicates the chunk doesn't exist, do don't load the data
    if(chunkOffset)
    {
        //Import chunk data and create chunk mesh
        unsigned char *buffer = new unsigned char[CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize];
        pRFile->seekg(CBYDefines::ChunkDataByteBoundarys * chunkOffset);
        pRFile->read((char *)buffer, CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize);

        for(int i = 0; i < CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize; ++i)
        {
            if(BlockType(buffer[i]) != BlockType::Air)
            {
                newChunk->blocks[i] = BlockType(buffer[i]);
                newChunk->numBlocks++;
            }
        }

        delete[] buffer;

        newChunk->CreateChunkMesh();
    }

    loadedChunks[HashChunkCoord(x, y, z)] = newChunk;
    return newChunk;
}

std::string TerrainManagerSystem::GetRegionFileName(unsigned __int64 hash)
{
    std::string fileName = "region" + std::to_string(hash) + ".dat";
    return fileName;
}

std::string TerrainManagerSystem::GetRegionFileNameAndPath(unsigned __int64 hash)
{
    return terrainDataPath + GetRegionFileName(hash);
}

void TerrainManagerSystem::CloseOpenRegionFiles()
{
    if(openRegionFiles.empty())return;
    //Close open region files
    for(auto& it : openRegionFiles)
    {
        it.second->close();
        delete it.second;
    }
    openRegionFiles.clear();
    LOGDEBUG("All open region files closed");
}
