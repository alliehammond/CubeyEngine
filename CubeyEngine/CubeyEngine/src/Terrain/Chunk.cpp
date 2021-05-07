#include "EnginePCH.h"
#include "Terrain\Chunk.h"
#include "Graphics\RenderComponent.h"

Chunk::Chunk(int xLoc, int yLoc, int zLoc) : x(xLoc), y(yLoc), z(zLoc)
{
    //Temporary chunk generation(lock y axis)
    for(int i = -ChunkSize / 2; i < ChunkSize / 2; ++i)
    {
        for(int j = -ChunkSize / 2; j < ChunkSize / 2; ++j)
        {
            GameObject *newObj = ObjectManagerSystem::CreateObject(new GameObject("ChunkBlock"));
            newObj->AddComponent<RenderComponent>(new RenderComponent("BasicCube.fbx", newObj));
            newObj->AddComponent<Transform>(new Transform(float(xLoc * ChunkSize + i * BlockSize), 
                float(yLoc * ChunkSize), 
                float(zLoc * ChunkSize + j * BlockSize), newObj));
        }
    }
    
    
}

Chunk::~Chunk()
{
    for(auto &it : blockTerrain)
        it->Delete();
}
