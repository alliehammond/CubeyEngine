#include "EnginePCH.h"
#include "Graphics\Mesh.h"

Mesh::Mesh(Material* mat) : material(0)
{
    if(mat)
    {
        material = mat->Clone();
    }
}

Mesh::Mesh() : material(0)
{ }

Mesh::~Mesh()
{
    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
    if(material)delete material;
}
