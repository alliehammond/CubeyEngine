#pragma once
#include "Graphics/Material.h"
#include "d3d11.h"
#include <string>

class Mesh
{
public:
    //Creates and stores a duplicate of the passed in material
    Mesh(Material *mat);
    Mesh();
    ~Mesh();

    Material *material;
private:
    ID3D11Buffer *vertexBuffer = 0, *indexBuffer = 0;
    UINT indexCount = 0;
    friend class Model;
    friend class GraphicsSystem;
    friend class Chunk;
};
