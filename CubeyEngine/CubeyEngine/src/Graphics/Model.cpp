#include "EnginePCH.h"
#include "Graphics\Model.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

const std::string modelPath = "../resources/models/";

Model::Model(std::string fileName, Material mat, ID3D11Device* d3ddevice)
{
    LoadModel(fileName, mat, d3ddevice);
}

Model::~Model()
{
    for(auto &it : meshes)
        delete it;
}

bool Model::LoadModel(std::string fileName, Material mat, ID3D11Device* d3ddevice)
{
    if(!meshes.empty())
    {
        for(auto& it : meshes)
            delete it;
    }
    std::string path = modelPath + fileName;
    std::string str = "Loading model from " + path;
    LOGDEBUG(str);

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded);

    //If the import failed, report it
    if(!scene) {
        LOGWARNING("Failed to load model from " + path);
        return false;
    }

    for(int i = 0;i < scene->mNumMeshes; ++i)
    {
        aiMesh *curMesh = scene->mMeshes[i];
        Mesh *newMesh = new Mesh();
        newMesh->material = mat;

        GraphicsSystem::VertexPosColor *vertices = new GraphicsSystem::VertexPosColor[curMesh->mNumVertices];
        //Load vertices from assimp aiMesh struct
        for(int j = 0;j < curMesh->mNumVertices; ++j)
        {
            vertices[j].color.x = (rand() % 100) / 100.0f;
            vertices[j].color.y = (rand() % 100) / 100.0f;
            vertices[j].color.z = (rand() % 100) / 100.0f;
            vertices[j].position.x = curMesh->mVertices[j].x;
            vertices[j].position.y = curMesh->mVertices[j].y;
            vertices[j].position.z = curMesh->mVertices[j].z;
        }

        //Create an initialize the vertex buffer.
        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.ByteWidth = sizeof(GraphicsSystem::VertexPosColor) * curMesh->mNumVertices;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

        resourceData.pSysMem = vertices;

        HRESULT hr = d3ddevice->CreateBuffer(&vertexBufferDesc, &resourceData, &d3dVertexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create vertex buffer!");
            return;
        }

        // Create and initialize the index buffer.
        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(_indices);
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        resourceData.pSysMem = _indices;

        hr = d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &d3dIndexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create index buffer!");
            return;
        }
        //ENDTEMP

        meshes.push_back(newMesh);
    }
    

    return true;
}
