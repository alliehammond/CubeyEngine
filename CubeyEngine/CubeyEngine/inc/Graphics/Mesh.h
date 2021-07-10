#pragma once
#include "Graphics/Material.h"
#include "d3d11.h"
#include <string>
#include <memory>

class Mesh
{
public:
    //Creates and stores a duplicate of the passed in material
    Mesh(std::unique_ptr<Material> mat);
    Mesh();
    ~Mesh();

    std::unique_ptr<Material> material;
private:
    std::unique_ptr<ID3D11Buffer> vertexBuffer, indexBuffer;
    UINT indexCount = 0;
    friend class Model;
    friend class GraphicsSystem;
    friend class Chunk;
};
