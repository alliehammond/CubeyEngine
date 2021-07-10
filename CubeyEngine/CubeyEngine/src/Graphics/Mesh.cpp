#include "EnginePCH.h"
#include "Graphics\Mesh.h"

Mesh::Mesh(std::unique_ptr<Material> mat)
{
    if(mat)
    {
        material = mat->Clone();
    }
}

Mesh::Mesh()
{ }

Mesh::~Mesh()
{ }
