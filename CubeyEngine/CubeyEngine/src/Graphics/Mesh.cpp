#include "EnginePCH.h"
#include "Graphics\Mesh.h"

//Loads first mesh in file (designed for single mesh files)
Mesh::Mesh()
{ }

Mesh::~Mesh()
{
    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
}
