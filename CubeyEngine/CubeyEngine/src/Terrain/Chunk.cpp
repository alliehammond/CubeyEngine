#include "EnginePCH.h"
#include "Terrain\Chunk.h"
#include "Graphics\RenderComponent.h"

Chunk::Chunk(int xLoc, int yLoc, int zLoc) : x(xLoc), y(yLoc), z(zLoc), blocks(ChunkSize * ChunkSize * ChunkSize)
{
    //Set all blocks to air
    for(int i = 0;i < ChunkSize * ChunkSize * ChunkSize; ++i)
        blocks[i] = BlockType::Air;

    LoadChunk();
    CreateChunkMesh();
}

Chunk::~Chunk()
{
    blockTerrain->Delete();
}

void Chunk::LoadChunk()
{
    //Temporary chunk generation(lock y axis)
    for(int i = 0; i < ChunkSize; ++i)
    {
        for(int j = 0; j < ChunkSize; ++j)
        {
            SetBlockChunkRelative(i, 0, j, BlockType::Dirt);
            ++numBlocks;
        }
    }
}

void Chunk::CreateChunkMesh()
{
    //Temp use hardcoded material
    Material mat;
    mat.name = "BasicMat";
    mat.pPixShader = GraphicsSystem::GetPixelShader("BasicPixelShader.cso");
    mat.pVertShader = GraphicsSystem::GetVertexShader("BasicVertexShader.cso");
    mat.pInputLayout = GraphicsSystem::GetInputLayout(InputLayout::POSCOL);

    Mesh* newMesh = new Mesh();
    newMesh->material = mat;
    //3 indices per face, 6 sides per cube, 2 faces per side of cube
    newMesh->indexCount = 3 * 6 * 2 * numBlocks;

    int totalFaces = numBlocks * 6 * 2, totalVerts = numBlocks * 8;

    GraphicsSystem::VertexPosColor* vertices = new GraphicsSystem::VertexPosColor[totalVerts];
    unsigned int* indices = new unsigned int[newMesh->indexCount];

    int curVertCount = 0;
    int curIndexCount = 0;

    for(unsigned int i = 0; i < ChunkSize; ++i)
    {
        for(unsigned int j = 0; j < ChunkSize; ++j)
        {
            for(unsigned int k = 0;k < ChunkSize; ++k)
            {
                if(GetBlockChunkRelative(i, j, k) == BlockType::Dirt)
                {
                    int blockStartVert = curVertCount;
                    //SORRY FOR THIS CODE I THINK ITS LESS COMPLEX THAN DOING IT WITH LOOPS
                    //Create the 8 vertices of each block
                    vertices[curVertCount + 0].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 0].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 0].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 1].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 1].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 1].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 2].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 2].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 2].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 3].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 3].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 3].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 4].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 4].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 4].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 5].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 5].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 5].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 6].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 6].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 6].color.z = (rand() % 100) / 100.0f;
                    vertices[curVertCount + 7].color.x = (rand() % 100) / 100.0f; vertices[curVertCount + 7].color.y = (rand() % 100) / 100.0f; vertices[curVertCount + 7].color.z = (rand() % 100) / 100.0f;

                    //Top 4 faces
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 1.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    //Bottom 4 faces                                                                                                                     
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 0.0f;
                    vertices[curVertCount].position.x = i + 0.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;
                    vertices[curVertCount].position.x = i + 1.0f; vertices[curVertCount].position.y = j + 0.0f; vertices[curVertCount++].position.z = k + 1.0f;

                    //Set indices for each face
                    //Top and bottom
                    indices[curIndexCount++] = blockStartVert + 0; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 2;
                    indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 3;
                    indices[curIndexCount++] = blockStartVert + 6; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 4;
                    indices[curIndexCount++] = blockStartVert + 7; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 6;
                    //Back and front                                                                                                   
                    indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 0;
                    indices[curIndexCount++] = blockStartVert + 4; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 0;
                    indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 3; indices[curIndexCount++] = blockStartVert + 7;
                    indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 7; indices[curIndexCount++] = blockStartVert + 6;
                    //Left and right                                                                                                   
                    indices[curIndexCount++] = blockStartVert + 0; indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 4;
                    indices[curIndexCount++] = blockStartVert + 2; indices[curIndexCount++] = blockStartVert + 6; indices[curIndexCount++] = blockStartVert + 4;
                    indices[curIndexCount++] = blockStartVert + 3; indices[curIndexCount++] = blockStartVert + 1; indices[curIndexCount++] = blockStartVert + 5;
                    indices[curIndexCount++] = blockStartVert + 3; indices[curIndexCount++] = blockStartVert + 5; indices[curIndexCount++] = blockStartVert + 7;
                }
            }
        }
    }


    //Create an initialize the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.ByteWidth = sizeof(GraphicsSystem::VertexPosColor) * totalVerts;
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

    Model *newModel = new Model();
    newModel->AddMesh(newMesh);

    blockTerrain = ObjectManagerSystem::CreateObject(new GameObject("BlockTerrainObj"));
    Transform *pTrans = blockTerrain->AddComponent<Transform>(new Transform(float(x * ChunkSize * BlockSize), float(y * ChunkSize * BlockSize), float(z * ChunkSize * BlockSize), blockTerrain));
    pTrans->scale = CBY::Vector(BlockSize, BlockSize, BlockSize);
    blockTerrain->AddComponent<RenderComponent>(new RenderComponent(newModel, blockTerrain));
}

BlockType Chunk::GetBlockChunkRelative(int x, int y, int z)
{
    return blocks[x + y * ChunkSize + z * ChunkSize * ChunkSize];
}

void Chunk::SetBlockChunkRelative(int x, int y, int z, BlockType type)
{
    blocks[x + y * ChunkSize + z * ChunkSize * ChunkSize] = type;
}
