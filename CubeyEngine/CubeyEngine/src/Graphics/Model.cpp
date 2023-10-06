#include "EnginePCH.h"
#include "Graphics\Model.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include "assimp\vector3.h"

const std::string modelPath = "../resources/models/";

Model::Model(std::string fileName, Material *mat, InputLayout IL, CBY::Vector4 colorAlpha)
{
    LoadModel(fileName, mat, IL, colorAlpha);
}

Model::~Model()
{
    ClearModel();
}

bool Model::LoadModel(std::string fileName, Material *mat, InputLayout IL, CBY::Vector4 colAlpha)
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
        aiProcess_MakeLeftHanded |
        aiProcess_FlipUVs);

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

        //Create and initialize the vertex buffer.
        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

        VertexPosCol *verticesPosCol = 0;
        VertexPosColAlpha *verticesPosColAlpha = 0;
        VertexPosUV *verticesPosUV = 0;

        if(IL == InputLayout::POSCOL)
        {
            verticesPosCol = new VertexPosCol[curMesh->mNumVertices];
            //Load vertices from assimp aiMesh struct
            for(unsigned int j = 0; j < curMesh->mNumVertices; ++j)
            {
                verticesPosCol[j].color.x = colAlpha.x;
                verticesPosCol[j].color.y = colAlpha.y;
                verticesPosCol[j].color.z = colAlpha.z;
                verticesPosCol[j].position.x = curMesh->mVertices[j].x;
                verticesPosCol[j].position.y = curMesh->mVertices[j].y;
                verticesPosCol[j].position.z = curMesh->mVertices[j].z;
            }
            vertexBufferDesc.ByteWidth = sizeof(VertexPosCol) * curMesh->mNumVertices;
            resourceData.pSysMem = verticesPosCol;
        }
        else if(IL == InputLayout::POSCOLALPHA)
        {
            verticesPosColAlpha = new VertexPosColAlpha[curMesh->mNumVertices];
            //Load vertices from assimp aiMesh struct
            for(unsigned int j = 0; j < curMesh->mNumVertices; ++j)
            {
                verticesPosColAlpha[j].colorAlpha.x = colAlpha.x;
                verticesPosColAlpha[j].colorAlpha.y = colAlpha.y;
                verticesPosColAlpha[j].colorAlpha.z = colAlpha.z;
                verticesPosColAlpha[j].colorAlpha.w = colAlpha.w;
                verticesPosColAlpha[j].position.x = curMesh->mVertices[j].x;
                verticesPosColAlpha[j].position.y = curMesh->mVertices[j].y;
                verticesPosColAlpha[j].position.z = curMesh->mVertices[j].z;
            }
            vertexBufferDesc.ByteWidth = sizeof(VertexPosColAlpha) * curMesh->mNumVertices;
            resourceData.pSysMem = verticesPosColAlpha;
        }
        else if(IL == InputLayout::POSUV)
        {
            verticesPosUV = new VertexPosUV[curMesh->mNumVertices];
            
            //Load vertices from assimp aiMesh struct
            for(unsigned int j = 0; j < curMesh->mNumVertices; ++j)
            {
                aiVector3D *texCoords = curMesh->mTextureCoords[0];
                verticesPosUV[j].uv.x = texCoords[j].x;
                verticesPosUV[j].uv.y = texCoords[j].y;
                verticesPosUV[j].position.x = curMesh->mVertices[j].x;
                verticesPosUV[j].position.y = curMesh->mVertices[j].y;
                verticesPosUV[j].position.z = curMesh->mVertices[j].z;
            }
            vertexBufferDesc.ByteWidth = sizeof(VertexPosUV) * curMesh->mNumVertices;
            resourceData.pSysMem = verticesPosUV;
        }
        else
        {
            LOGERROR("Invalid input layout passed to model loader!");
            return false;
        }

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

        std::string str = "Loaded mesh from " + path + "! V:" + std::to_string(totalVerts) + " F:" + std::to_string(totalFaces);
        LOGDEBUG(str);

        hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &newMesh->indexBuffer);
        if(FAILED(hr))
        {
            LOGERROR("Failed to create index buffer!");
            return false;
        }
        
        if(verticesPosCol)delete[] verticesPosCol;
        if(verticesPosUV)delete[] verticesPosUV;
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

Mesh *Model::GetMesh(unsigned index)
{
    if(index >= meshes.size())return 0;
    return meshes[index];
}