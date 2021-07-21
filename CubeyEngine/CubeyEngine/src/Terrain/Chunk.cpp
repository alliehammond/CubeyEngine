#include "EnginePCH.h"
#include "Terrain\Chunk.h"
#include "Graphics\RenderComponent.h"
#include "Graphics\Materials\TextureMaterial.h"

Chunk::Chunk(short xLoc, short yLoc, short zLoc, bool empty_) : x(xLoc), y(yLoc), z(zLoc), empty(empty_), blockTerrain(0)
{
    if(!empty)
    {
        blocks.resize(CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize);
        std::fill(blocks.begin(), blocks.end(), BlockType::Air);
    }
}

Chunk::~Chunk()
{
    if(blockTerrain)blockTerrain->Delete();
}

//Generate a chunk without loading chunk data
void Chunk::LoadChunk()
{
    //Generate floor of blocks
    for(int i = 0; i < CBYDefines::ChunkSize; ++i)
    {
        for(int j = 0; j < CBYDefines::ChunkSize; ++j)
        {
            SetBlockChunkRelative(i, 0, j, BlockType::Dirt);
            ++numBlocks;
        }
    }
}

//Creates the chunk mesh once blocks vector has been filled and numBlocks has been set to number of non-air blocks
void Chunk::CreateChunkMesh()
{
    if(empty)return;

    //Clear chunk model if it already exists
    if(blockTerrain)
    {
        Model *pModel = blockTerrain->GetComponent<RenderComponent>()->pModel;
        pModel->ClearModel();
    }

    //Temp use hardcoded material
    TextureMaterial mat("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUV, "BaseTextureMaterial", "dirtTexture.tga");

    Mesh* newMesh = new Mesh(&mat);
    //3 indices per face, 6 sides per cube, 2 faces per side of cube
    newMesh->indexCount = 3 * 6 * 2 * numBlocks;

    int totalFaces = numBlocks * 6 * 2, totalVerts = numBlocks * 24;

    VertexPosUV* vertices = new VertexPosUV[totalVerts];
    unsigned int* indices = new unsigned int[newMesh->indexCount];

    int curVertCount = 0;
    int curIndexCount = 0;

    for(unsigned int i = 0; i < CBYDefines::ChunkSize; ++i)
    {
        for(unsigned int j = 0; j < CBYDefines::ChunkSize; ++j)
        {
            for(unsigned int k = 0;k < CBYDefines::ChunkSize; ++k)
            {
                if(GetBlockChunkRelative(i, j, k) != BlockType::Air)
                {
                    int blockStartVert = curVertCount;


                    //SORRY FOR THIS CODE I THINK ITS LESS COMPLEX THAN DOING IT WITH LOOPS
                    //Create the 24 vertices of each block and assign UVs

                    //Top 2 faces (0-3)
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    //Bottom 2 faces (4-7)
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    //Front 2 faces (8-11)
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    //Back 2 faces (12-15)
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    //Left 2 faces (16-19)
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    //Right 2 faces (20-23)
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 0.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].uv = XMFLOAT2(0.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].uv = XMFLOAT2(1.0f, 1.0f);
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;

                    //Set indices for each face
                    //Top and bottom
                    indices[curIndexCount++] = blockStartVert + 0; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 2;
                    indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 3;
                    indices[curIndexCount++] = blockStartVert + 6; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 4;
                    indices[curIndexCount++] = blockStartVert + 7; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 6;
                    //Front and back                                                                                                 
                    indices[curIndexCount++] = blockStartVert + 11; indices[curIndexCount++] = blockStartVert + 9; indices[curIndexCount++] = blockStartVert + 8;
                    indices[curIndexCount++] = blockStartVert + 10; indices[curIndexCount++] = blockStartVert + 11; indices[curIndexCount++] = blockStartVert + 8;
                    indices[curIndexCount++] = blockStartVert + 12; indices[curIndexCount++] = blockStartVert + 13; indices[curIndexCount++] = blockStartVert + 15;
                    indices[curIndexCount++] = blockStartVert + 12; indices[curIndexCount++] = blockStartVert + 15; indices[curIndexCount++] = blockStartVert + 14;
                    //Left and right                                                                                                   
                    indices[curIndexCount++] = blockStartVert + 16; indices[curIndexCount++] = blockStartVert + 17; indices[curIndexCount++] = blockStartVert + 18;
                    indices[curIndexCount++] = blockStartVert + 17; indices[curIndexCount++] = blockStartVert + 19; indices[curIndexCount++] = blockStartVert + 18;
                    indices[curIndexCount++] = blockStartVert + 20; indices[curIndexCount++] = blockStartVert + 22; indices[curIndexCount++] = blockStartVert + 21;
                    indices[curIndexCount++] = blockStartVert + 21; indices[curIndexCount++] = blockStartVert + 22; indices[curIndexCount++] = blockStartVert + 23;
                }
            }
        }
    }


    //Create an initialize the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(VertexPosUV) * totalVerts;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

    resourceData.pSysMem = vertices;

    HRESULT hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &resourceData, &newMesh->vertexBuffer);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create vertex buffer!");
        return;
    }

    

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * newMesh->indexCount;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    resourceData.pSysMem = indices;

    std::string str = "Loaded chunk " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "! V:" + std::to_string(totalVerts) + " F:" + std::to_string(totalFaces);
    LOGDEBUG(str);

    hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &newMesh->indexBuffer);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create index buffer!");
        return;
    }

    delete[] vertices;
    delete[] indices;

    if(!blockTerrain)
    {
        Model* newModel = new Model();
        newModel->AddMesh(newMesh);
        blockTerrain = ObjectManagerSystem::CreateObject(new GameObject("BlockTerrainObj"));
        Transform *pTrans = blockTerrain->GetComponent<Transform>();
        pTrans->pos = CBY::Vector(float(x * CBYDefines::ChunkSize * CBYDefines::BlockSize), float(y * CBYDefines::ChunkSize * CBYDefines::BlockSize), float(z * CBYDefines::ChunkSize * CBYDefines::BlockSize));
        pTrans->scale = CBY::Vector(CBYDefines::BlockSize, CBYDefines::BlockSize, CBYDefines::BlockSize);
        blockTerrain->AddComponent<RenderComponent>(new RenderComponent(newModel, blockTerrain));
    }
    else
    {
        blockTerrain->GetComponent<RenderComponent>()->pModel->AddMesh(newMesh);
    }
}

BlockType Chunk::GetBlockChunkRelative(short x, short y, short z)
{
    if(empty)return BlockType::Air;
    return blocks[x + y * CBYDefines::ChunkSize + z * CBYDefines::ChunkSize * CBYDefines::ChunkSize];
}

//Check if chunk is empty and if so resize blocks vector/set empty to false, also check if adding a non air block and inc/dec numBlocks
void Chunk::SetBlock(short x, short y, short z, BlockType type, bool regenMesh)
{
    if(type == BlockType::Air && empty)return;
    if(empty)
    {
        //Chunk is empty, create blocks vector
        empty = false;
        blocks.resize(CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize);
        std::fill(blocks.begin(), blocks.end(), BlockType::Air);
    }
    BlockType curType = GetBlockChunkRelative(x, y, z);
    if(curType == type)return;
    if(curType == BlockType::Air)++numBlocks;
    if(curType != BlockType::Air && type == BlockType::Air)--numBlocks;
    SetBlockChunkRelative(x, y, z, type);

    if(regenMesh)CreateChunkMesh();
}

//Don't use this function to set blocks in empty chunk
void Chunk::SetBlockChunkRelative(short x, short y, short z, BlockType type)
{
    blocks[x + y * CBYDefines::ChunkSize + z * CBYDefines::ChunkSize * CBYDefines::ChunkSize] = type;
}
