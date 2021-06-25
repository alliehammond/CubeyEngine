#include "EnginePCH.h"
#include "Graphics\Model.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

const std::string modelPath = "../resources/models/";

Model::Model(std::string fileName, Material *mat)
{
    LoadModel(fileName, mat);
}

Model::~Model()
{
    ClearModel();
}

bool Model::LoadModel(std::string fileName, Material *mat)
{
    if(!meshes.empty())
    {
        for(auto& it : meshes)
            delete it;
    }
    std::string path = modelPath + fileName;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded);

    //If the import failed, report it
    if(!scene) {
        LOGWARNING("Failed to load model from " + path);
        return false;
    }
    unsigned int totalVerts = 0, totalFaces = 0;
    for(unsigned int i = 0;i < scene->mNumMeshes; ++i)
    {
        aiMesh *curMesh = scene->mMeshes[i];
        Mesh *newMesh = new Mesh(mat);
        //Assume 3 indices per face
        newMesh->indexCount = 3 * curMesh->mNumFaces;
        totalFaces += curMesh->mNumFaces;
        totalVerts += curMesh->mNumVertices;

        VertexPosColor *vertices = new VertexPosColor[curMesh->mNumVertices];
        //Load vertices from assimp aiMesh struct
        for(unsigned int j = 0;j < curMesh->mNumVertices; ++j)
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
        vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * curMesh->mNumVertices;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

        resourceData.pSysMem = vertices;

        HRESULT hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &resourceData, &newMesh->vertexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create vertex buffer!");
            return false;
        }

        //Create and initialize the index buffer
        unsigned int *indices = new unsigned int[curMesh->mNumFaces * 3];
        for(unsigned int j = 0; j < curMesh->mNumFaces; ++j)
        {
            aiFace *curFace = &(curMesh->mFaces[j]);
            //Assume there are 3 indices per face
            indices[0 + j * 3] = curFace->mIndices[0];
            indices[1 + j * 3] = curFace->mIndices[1];
            indices[2 + j * 3] = curFace->mIndices[2];
        }

        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.ByteWidth = sizeof(unsigned int) * curMesh->mNumFaces * 3;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        resourceData.pSysMem = indices;

        std::string str = "Loaded model from " + path + "! V:" + std::to_string(totalVerts) + " F:" + std::to_string(totalFaces);
        LOGDEBUG(str);

        hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &newMesh->indexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create index buffer!");
            return false;
        }
        
        delete[] vertices;
        delete[] indices;

        meshes.push_back(newMesh);
    }

    return true;
}

Model::Model()
{ }

void Model::AddMesh(Mesh *pMesh)
{
    meshes.push_back(pMesh);
}

void Model::ClearModel()
{
    for(auto& it : meshes)
        delete it;
    meshes.clear();
}
