#include "EnginePCH.h"
#include "Terrain\Chunk.h"
#include "Graphics\RenderComponent.h"

Chunk::Chunk(short xLoc, short yLoc, short zLoc, bool empty_) : x(xLoc), y(yLoc), z(zLoc), empty(empty_), blockTerrain(0)
{
    //Set all blocks to air
    std::fill(blocks.begin(), blocks.end(), BlockType::Air);
    if(!empty)blocks.resize(CBYDefines::ChunkSize * CBYDefines::ChunkSize * CBYDefines::ChunkSize);
}

Chunk::~Chunk()
{
    if(blockTerrain)blockTerrain->Delete();
}

//Temp for generating chunk without loading chunk data
void Chunk::LoadChunk()
{
    //Temporary chunk generation(lock y axis)
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

    for(unsigned int i = 0; i < CBYDefines::ChunkSize; ++i)
    {
        for(unsigned int j = 0; j < CBYDefines::ChunkSize; ++j)
        {
            for(unsigned int k = 0;k < CBYDefines::ChunkSize; ++k)
            {
                if(GetBlockChunkRelative(i, j, k) != BlockType::Air)
                {
                    int blockStartVert = curVertCount;
                    //Assign vertex colors randomly
                    float colr = (rand() % 100) / 100.0f, colg = 0.0f, colb = colr;
                    for(unsigned int z = 0; z < 8; ++z)
                    {
                        vertices[curVertCount + z].color.x = colr; vertices[curVertCount + z].color.y = colg; vertices[curVertCount + z].color.z = colb;
                    }
                    //SORRY FOR THIS CODE I THINK ITS LESS COMPLEX THAN DOING IT WITH LOOPS
                    //Create the 8 vertices of each block

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
    Transform *pTrans = blockTerrain->AddComponent<Transform>
        (new Transform(float(x * CBYDefines::ChunkSize * CBYDefines::BlockSize), float(y * CBYDefines::ChunkSize * CBYDefines::BlockSize), float(z * CBYDefines::ChunkSize * CBYDefines::BlockSize), blockTerrain));
    pTrans->scale = CBY::Vector(CBYDefines::BlockSize, CBYDefines::BlockSize, CBYDefines::BlockSize);
    blockTerrain->AddComponent<RenderComponent>(new RenderComponent(newModel, blockTerrain));
}

BlockType Chunk::GetBlockChunkRelative(short x, short y, short z)
{
    if(empty)return BlockType::Air;
    return blocks[x + y * CBYDefines::ChunkSize + z * CBYDefines::ChunkSize * CBYDefines::ChunkSize];
}

//Don't use this function to set blocks in empty chunk
void Chunk::SetBlockChunkRelative(short x, short y, short z, BlockType type)
{
    blocks[x + y * CBYDefines::ChunkSize + z * CBYDefines::ChunkSize * CBYDefines::ChunkSize] = type;
}
