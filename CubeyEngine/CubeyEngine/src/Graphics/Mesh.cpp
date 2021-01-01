#include "EnginePCH.h"
#include "Graphics\Mesh.h"

//Loads firsst mesh in file (designed for single mesh files)
Mesh::Mesh(std::string fileName, Material mat)
{
    if(vertexBuffer)SafeRelease(vertexBuffer);
    if(indexBuffer)SafeRelease(indexBuffer);
}

Mesh::~Mesh()
{
    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
}

//Returns true on success, if a mesh is already loaded deletes that first
bool Mesh::LoadMesh(std::string fileName, Material mat)
{
    return true;
}
