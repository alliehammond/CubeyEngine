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

    //Vector for the vertices and indices of each block type
    std::vector<std::vector<VertexPosUVNorm>> blockTypeVertices;
    std::vector<std::vector<unsigned int>> blockTypeIndices;

    //Initialize vectors
    for(unsigned char i = 0;i < unsigned char(BlockType::BLOCKCOUNT); ++i)
    {
        blockTypeVertices.push_back(std::vector<VertexPosUVNorm>());
        blockTypeIndices.push_back(std::vector<unsigned int>());
    }

    for(unsigned int i = 0; i < CBYDefines::ChunkSize; ++i)
    {
        for(unsigned int j = 0; j < CBYDefines::ChunkSize; ++j)
        {
            for(unsigned int k = 0;k < CBYDefines::ChunkSize; ++k)
            {
                BlockType bType = GetBlockChunkRelative(i, j, k);
                if(bType != BlockType::Air)
                {
                    size_t bTypeIndex = size_t(bType);
                    int blockStartVert = int(blockTypeVertices[bTypeIndex].size());
                    

                    //SORRY FOR THIS CODE I THINK ITS LESS COMPLEX THAN DOING IT WITH LOOPS
                    //Create the 24 vertices of each block and assign UVs

                    //Top 2 faces (0-3)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });

                    //Bottom 2 faces (4-7)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) });

                    //Front 2 faces (8-11)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) });

                    //Back 2 faces (12-15)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) });

                    //Left 2 faces (16-19)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 1.0f, k + 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 0.0f, j + 0.0f, k + 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) });

                    //Right 2 faces (20-23)
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 1.0f, k + 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) });
                    blockTypeVertices[bTypeIndex].push_back(VertexPosUVNorm{ XMFLOAT3(i + 1.0f, j + 0.0f, k + 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) });

                    //Set indices for each face
                    //Top and bottom
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 0); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 1); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 2);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 2); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 1); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 3);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 6); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 5); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 4);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 7); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 5); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 6);

                    //Front and back                            
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 11); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 9); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 8);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 10); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 11); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 8);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 12); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 13); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 15);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 12); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 15); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 14);

                    //Left and right                                                                                  
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 16); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 17); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 18);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 17); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 19); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 18);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 20); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 22); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 21);
                    blockTypeIndices[bTypeIndex].push_back(blockStartVert + 21); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 22); blockTypeIndices[bTypeIndex].push_back(blockStartVert + 23);
                }
            }
        }
    }

    int totalChunkVerts = 0;

    //Loop through each vector and create meshes for each one that contains blocks (start at 1 to skip air blocks)
    for(unsigned char i = 1; i < unsigned char(BlockType::BLOCKCOUNT); ++i)
    {
        BlockType bType = BlockType(i);

        //Skip empty buffers
        if(blockTypeVertices[i].empty())continue;

        //Create an initialize the vertex buffer.
        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.ByteWidth = sizeof(VertexPosUVNorm) * unsigned(blockTypeVertices[i].size());
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        
        resourceData.pSysMem = blockTypeVertices[i].data();

        Mesh *newMesh = 0;
        switch(bType)
        {
            case BlockType::Dirt:
            {
                TextureMaterial mat("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "dirtTexture.tga");
                newMesh = new Mesh(&mat);
                break;
            }
            case BlockType::Grass:
            {
                TextureMaterial mat("BasicTextureTopSideBottomVS.cso", "BasicTextureTopSideBottomPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "grassTexture.tga");
                newMesh = new Mesh(&mat);
                break;
            }
            case BlockType::Stone:
            {
                TextureMaterial mat("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "stoneTexture.tga");
                newMesh = new Mesh(&mat);
                break;
            }
            default:
                LOGERROR("Invalid block type attempting to be loaded!");
        }

        HRESULT hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &resourceData, &newMesh->vertexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create vertex buffer!");
            return;
        }

        newMesh->indexCount = unsigned(blockTypeIndices[i].size());

        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.ByteWidth = sizeof(unsigned int) * newMesh->indexCount;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        resourceData.pSysMem = blockTypeIndices[i].data();

        

        hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &newMesh->indexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create index buffer!");
            return;
        }

        if(!blockTerrain)
        {
            Model *newModel = new Model();
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

        totalChunkVerts += unsigned(blockTypeVertices[i].size());
    }

    std::string str = "Loaded chunk " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "! V:" + std::to_string(totalChunkVerts);
    LOGDEBUG(str);
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
